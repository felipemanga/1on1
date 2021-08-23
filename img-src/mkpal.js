const fs = require("fs");

// let colors = [];
// for (let i = 0; i<256; ++i) {
//     colors.push(`        colorFromRGB(${i}, ${i}, ${i})`);
// }

// let out = `
// #pragma once
// #include "Femto"

// namespace Graphics {

//     inline constexpr u16 generalPalette[] = {
// ${colors.join(",\n")}
//     };

// }
// `
// console.log(out);

let zs = [];
let z = 5;
let zacc = 0.5;
for (let i = 0; i < 128; ++i) {
    zs.push(z);
    z += zacc;
    zacc += 0.008;
}

let prev = 0;
fs.writeFileSync("../src/ztables.h", `
constexpr inline const u16 ztable[] = {
${zs.map(z=>{
let nz = z*256|0;
let ret = nz - prev;
prev = nz;
return nz; // ret;
}).join(", ")}
};

constexpr inline const s16 iztable[] = {
${zs.map(z=>(40.0/z)*256|0).join(", ")}
};
`, "utf-8");

const palettes = {
    map1: {
        controls:{
            0: "#5b7686",
            1: "#6c7d88",
            4: "#bccde8",
            7: "#6c7d88",
            8: "#907868",
            10: "#5b453b",
            11: "#49593a",
            12: "#394930",
            13: "#b77f73",
            19: "#a77f73",
            20: "#49593a",
            21: "#5b453b",
            254: "#6c5247",
            255: "#ffdead"
        },
        noise:[
            {min:11, max:18, weight:0.13},
            {min:20, max:255, weight:0.13}
        ]
    }
};

for (let name in palettes) {
    mkpalette(name, palettes[name]);
}

function mkpalette(name, config) {
    const {controls, noise} = config;

    let pal = [[0,0,0]];
    for(let key in controls) {
        pal[key|0] = hexToRgb(controls[key]);
    }
    let prevC = [0,0,0], prevI = 0;
    let nextC, nextI = 0;
    for(let i = 0; i < pal.length; ++i) {
        let c = pal[i];
        if (c) {
            prevC = c;
            prevI = i;
            nextC = null;
            continue;
        }
        if (!nextC) {
            for(nextI = i + 1; nextI < 256 && !nextC; ++nextI) {
                nextC = pal[nextI];
            }
        }
        let w = (i - prevI) / (nextI - prevI);
        let iw = 1 - w;
        pal[i] = [
            (prevC[0] * iw + nextC[0] * w),
            (prevC[1] * iw + nextC[1] * w),
            (prevC[2] * iw + nextC[2] * w)
        ];
    }

    for(let {min, max, weight} of noise) {
        for (let i = min; i < max; ++i) {
            pal[i] = [
                Math.min(255, pal[i][0] * (Math.random() * weight + (1 - weight))),
                Math.min(255, pal[i][1] * (Math.random() * weight + (1 - weight))),
                Math.min(255, pal[i][2] * (Math.random() * weight + (1 - weight)))
            ];
        }
    }

    for(let i = 1; i < pal.length; i += 2) {
        let l = (i / pal.length) * 0.75 + 0.75;
        if (l >= 1) continue;
        pal[i][0] *= l;
        pal[i][1] *= l;
        pal[i][2] *= l;
    }

    fs.writeFileSync(name + ".h", `
#pragma once
inline u16 map1[] = {
${pal.map(c=>"  colorFromRGB(0x" + c.map(c=>(c|0).toString(16)).join("") + ")").join(",\n")}
};
`, "utf-8");
}

function hexToRgb(hex) {
    if (typeof hex == "string") hex = parseInt(hex.substr(1), 16);
    return [hex >> 8 >> 8, (hex >> 8) & 0xFF, hex & 0xFF];
}
