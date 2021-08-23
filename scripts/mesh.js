let palette;

let textures = {};

APP.getPalette(pal=>{
    palette = pal;
    let meshes = dir("meshes")
        .filter( name=>/\.obj$/i.test(name) )
        //.filter( f=>f=="Jet.obj")
        .map( name => {
            try {
                return parseObj(read(`meshes/${name}`), name);
            } catch(ex){
                APP.error("Error parsing " + name + ex);
            }
        });

    loadTextures(_=>{
        meshes.forEach( mesh => {
            if (mesh) {
                exportMesh(mesh);
            }
        });
    });
});

function loadTextures(cb) {
    let count = 0;
    for (let fileName in textures) {
        let image = textures[fileName];
        if (image) continue;
        count++;
        readImage("meshes/" + fileName)
            .then(registerTexture.bind(null, fileName));
        log("Loading ", fileName);
    }

    if (!count)
        cb();

    function registerTexture(name, data) {
        count--;
        textures[name] = data;
        if (!count)
            cb();
    }
}

function kv( str ){
    return str
        .split("\n")
        .map(line=>line.trim().split(/\s+/))
        .reduce((obj, line)=>{
            let cmd = line.shift();
            if( cmd.length && cmd != "#"){
                (obj[cmd] = obj[cmd] || []).push(line);
                obj[cmd.toLowerCase()] = obj[cmd.toLowerCase()] || obj[cmd];
            }
            return obj;
        }, {});
}

function parseObj(str, name){
    let mtllib = {};
    let cmtl = null;
    
    let mesh = str
        .split("\n")
        .map(line=>line.trim().split(/\s+/))
        .reduce((obj, line)=>{
            let cmd = line.shift();
            if( cmd == "mtllib" ){
                
                read(`meshes/${line[0]}`)
                    .split(/\n(?=newmtl )/)
                    .forEach(str=>{
                        const mtl = kv(str);
                        if( mtl.newmtl )
                            mtllib[mtl.newmtl[0]] = mtl;
                        if( mtl.map_kd )
                            textures[mtl.map_kd] = null;
                    });
                    
            } else if( cmd == "usemtl" ) {
                
                cmtl = mtllib[line[0]];
                
            } else if( cmd == "f" ) {

                let c = 0;
                if( cmtl ){
                    if( !("c" in cmtl) ){
                        cmtl.c = RGB(... (cmtl.kd[0].map(x=>255*x)) )|0;
                    }
                    c = cmtl.c;
                }

                line = line.map(p => p.split("/").map(i=>i|0) );
                line.c = c;
                line.map_kd = cmtl.map_kd;
            }
            
            (obj[cmd] = obj[cmd] || []).push(line);
            return obj;
        }, {});
    
    if(mesh.vn)
        mesh.vn = mesh.vn.map(v=>v.map(f=>parseFloat(f)));
        
    mesh.v = mesh.v.map(v=>v.map(f=>parseFloat(f)));

    mesh.name = name;

    return mesh;
}

function triangulate(faces) {
    for (let i = 0; i < faces.length; ++i) {
        let face = faces[i];
        if (face.length > 3) {
            let n = face.splice(0, 3, face[1], face[2]);
            n.c = face.c;
            n.map_kd = face.map_kd;
            faces.push(n);
            i--;
        }
    }
    return faces;
}

function exportMesh(mesh){
    let minX=0xFFFFFF, maxX = -0xFFFFFF;
    let minY=0xFFFFFF, maxY = -0xFFFFFF;
    let minZ=0xFFFFFF, maxZ = -0xFFFFFF;
    let remap = [];
    let cullCount = 0;

    mesh.v.forEach(([x, y, z], i)=>{
        if( x < minX ) minX = x;
        if( x > maxX ) maxX = x;
        if( y < minY ) minY = y;
        if( y > maxY ) maxY = y;
        if( z < minZ ) minZ = z;
        if( z > maxZ ) maxZ = z;
    });

    let scaleX = 127 / (maxX - minX);
    let scaleY = 127 / (maxY - minY);
    let scaleZ = 127 / (maxZ - minZ);
    
    scaleX = scaleY = scaleZ = Math.min(scaleX, scaleY, scaleZ);

    mesh.v.forEach((a, i)=>{
        let [x, y, z] = a;
        x = ((x - minX) - (maxX - minX)/2) * scaleX;
        y = ((y - minY) - (maxY - minY)/2) * scaleY;
        z = ((z - minZ) - (maxZ - minZ)/2) * scaleZ;
        if( x < -128 || x > 127 ) log("Invalid X: " + x);
        if( y < -128 || y > 127 ) log("Invalid Y: " + y);
        if( z < -128 || z > 127 ) log("Invalid Z: " + z);
        a[0] = x;
        a[1] = y;
        a[2] = z;
    });

    mesh.f = triangulate(mesh.f);

    mesh.f = mesh.f.filter((p, fi)=>{
        const bad = p.findIndex(([i])=>mesh.v[i-1] === undefined);
        if( bad != -1 ){
            throw `${name} bad index: ${JSON.stringify(p)}[${bad}] on face ${fi}`;
        }

        let minX=0xFFFFFF, maxX = -0xFFFFFF;
        let minY=0xFFFFFF, maxY = -0xFFFFFF;
        let minZ=0xFFFFFF, maxZ = -0xFFFFFF;
        
        let any = 0, nyc = 0;
        
        p.y = 0;
        p.forEach(([i, vti, vni])=>{
            let [x, y, z] = mesh.v[i-1];
            if (mesh.vn) {
                let [nx, ny, nz] = mesh.vn[vni-1];
                any += ny;
                nyc++;
            }
            if( x < minX ) minX = x;
            if( x > maxX ) maxX = x;
            if( y < minY ) minY = y;
            if( y > maxY ) maxY = y;
            if( z < minZ ) minZ = z;
            if( z > maxZ ) maxZ = z;
            p.y += y;
        });
        
        if (!mesh.vn) {
            let A = mesh.v[p[0][0] - 1];
            let B = mesh.v[p[1][0] - 1];
            let C = mesh.v[p[2][0] - 1];
            any = (A[2] - C[2]) * (B[0] - C[0]) - (A[0] - C[0]) * (B[2] - C[2]);
        }
        
        any = 1;
        if (any < 0) {
            cullCount++;
            return false;
        }

        // if ((maxX - minX) + (maxY - minY) + (maxZ - minZ) < 12){
        if (Math.min((maxX - minX), (maxY - minY), (maxZ - minZ)) < 0) {
            cullCount++;
            return false;
        }

        p.forEach(([i])=>{
            mesh.v[i-1].push(true);
        });

        return true;
    });
    
    let newI = 0;
    mesh.v = mesh.v.filter( ([x, y, z, u], i)=>{
        remap[i] = newI;
        if( !u ) return false;
        newI++;
        return true;
    });
    
    const name = mesh.name.replace(/\.obj/g, '');

    if(mesh.f.length > 0xFFFF){
        log(`Too many faces in ${name}: ${mesh.f.length}`);
        mesh.f.length = 0xFFFF;
        return;
    }

    const vtxSize = 3;
    const totalSize = 3 + mesh.f.length * 4 + mesh.v.length * vtxSize;

    const bytes = new Uint8ClampedArray(totalSize);
    const sbytes = new Int8Array(bytes.buffer);
    
    let p = 0;
    bytes[p++] = mesh.f.length >> 8;
    bytes[p++] = mesh.f.length & 0xFF;
    bytes[p++] = mesh.v.length;
    
    mesh.f = mesh.f.sort((a, b) => a.y - b.y);

    let error = false;
    
    mesh.f.forEach(f=>{
        let color = f.c|0;

        if (textures[f.map_kd]) {
            let tex = textures[f.map_kd];
            //let u = Math.round(parseFloat(mesh.vt[f[0][1]-1][0]) * tex.width);
            //let v = Math.round((1.0 - parseFloat(mesh.vt[f[0][1]-1][1])) * tex.height);
            let u = 0, v = 0, sum = 0;
            f.forEach(([i, uvi, ni], index)=>{
                u += parseFloat(mesh.vt[uvi - 1][0]);
                v += 1 - parseFloat(mesh.vt[uvi - 1][1]);
                sum++;
            });
            if (sum > 1) {
                u /= sum;
                v /= sum;
            }
            u *= tex.width;
            v *= tex.height;
            let i = Math.round(u) + Math.round(v) * tex.width;
            i *= 4;
            let r = tex.data[i++]|0;
            let g = tex.data[i++]|0;
            let b = tex.data[i++]|0;
            color = RGB(r, g, b);
        }
        bytes[p++] = color|0;

        f.forEach(([i, uvi, ni], index)=>{
            if( i-1 > 255 && !error ){
                error = true;
                APP.error(`I overflow in ${name}: ${i} with ${cullCount} culled.`);
            }
            if( index >= 3 ){
                throw new Error(`${name} not triangulated: ${f.length}`);
            } 
            bytes[p++] = remap[i - 1];
        });
    });

    if (error)
        return;
    
    let v0 = p;
    
    mesh.v.forEach(([x, y, z], i)=>{
        sbytes[p++] = Math.round(x);
        sbytes[p++] = Math.round(y);
        sbytes[p++] = Math.round(z);
    });

    if( p != bytes.length )
        log(name + " size mismatch:", p, bytes.length);
        
    log("Exporting ", name, " culled ", cullCount, " faces:", mesh.f.length);
    write(`meshes/${name[0].toUpperCase() + name.substr(1)}.h`, `constexpr inline u8 ${name}[] = {\n${Array.from(bytes).join(", ")}\n};\n`);
    // log(`Wrote: meshes/${name[0].toUpperCase() + name.substr(1)}.bin`)
}

function RGB(r, g, b){
    let closestI = 0;
    let closestD = 0x7FFFFFFF;
    for(let i=1; i<palette.length; i++ ){
        let [cr, cg, cb] = palette[i];
        cr -= r; cg -= g; cb -= b;
        let d = cr*cr + cg*cg + cb*cb;
        if( d < closestD ){
            closestD = d;
            closestI = i;
        }
    }
    return closestI;
}
