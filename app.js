'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 
let dbConf = {
	host     : 'dursley.socs.uoguelph.ca',
	user     : '',
	password : '',
	database : ''
};
const mysql = require('mysql2/promise');

let sharedLib = ffi.Library(__dirname+'/./libgpxparser', {
  'GPXtoJSONWrapper': [ 'string', ['string','string']],
  'RoutetoJSONWrapper': [ 'string', ['string','string']],
  'TracktoJSONWrapper': [ 'string', ['string','string']],
  'ExtratoJSONWrapper': [ 'string', ['string','string','string']],
  'reName': [ 'string', ['string','string','string','string']],
  'JSONCreateGPXWrapper': [ 'string', ['string','string','string']],
  'JSONCreateRoute': [ 'string', ['string','string','string']],
  'JSONCreateWay': [ 'string', ['string','string','string','string']],
  'JSONBetweenRoute': [ 'string', ['string','string','float','float','float','float','float']],
  'JSONBetweenTrack': [ 'string', ['string','string','float','float','float','float','float']],
  'RouteLength': [ 'int', ['string','string','float','float']],
  'TrackLength': [ 'int', ['string','string','float','float']],
  'validateWrapper': [ 'int', ['string','string']],
  'JSONGetWay': [ 'string', ['string','string','string']],
});

app.get('/login', async function(req , res){
  
  dbConf.user = req.query.data1;
  dbConf.password = req.query.data2;
  dbConf.database = req.query.data3;
  let retStr = 1;
  try {
    let connection = await mysql.createConnection(dbConf);
    connection.close();
  } catch(e) {
    retStr = 0;
  }
  res.send(
    {
      ret: retStr
    }
  );
});

app.get('/Store', async function(req , res){
  
  let retStr = 1;
  try {
    let connection = await mysql.createConnection(dbConf);
    let sql = 'CREATE TABLE IF NOT EXISTS FILE(gpx_id INT AUTO_INCREMENT, file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) NOT NULL, PRIMARY KEY (gpx_id))';
    await connection.execute(sql);
    sql = 'CREATE TABLE IF NOT EXISTS ROUTE(route_id INT AUTO_INCREMENT, route_name VARCHAR(60), route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL,PRIMARY KEY (route_id), FOREIGN KEY (gpx_id) REFERENCES FILE (gpx_id) ON DELETE CASCADE)';
    await connection.execute(sql);
    sql = 'CREATE TABLE IF NOT EXISTS POINT(point_id INT AUTO_INCREMENT, point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(60), route_id INT NOT NULL, PRIMARY KEY (point_id), FOREIGN KEY (route_id) REFERENCES ROUTE (route_id) ON DELETE CASCADE)';
    await connection.execute(sql);

    await connection.execute("DELETE FROM FILE");
    await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 1");
    let retStr = fs.readdirSync(__dirname+'/uploads');
    retStr.forEach(async retStr => {
      let str = sharedLib.GPXtoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
      if (str != "{}") {
        let json = JSON.parse(str);
        let insRec = 'INSERT INTO FILE (file_name, ver, creator) VALUES (\''+retStr+'\','+json.version+',\''+json.creator+'\')';
        await connection.execute(insRec);
      }
    })
    
    let j = 0;
    await connection.execute("ALTER TABLE ROUTE AUTO_INCREMENT = 1");
    await connection.execute("ALTER TABLE POINT AUTO_INCREMENT = 1");
    const [rows1, fields1] = await connection.execute('SELECT * from `FILE` ORDER BY `gpx_id`');
    for (let row of rows1){
      let route = sharedLib.RoutetoJSONWrapper(__dirname+'/uploads/'+row.file_name,__dirname+'/parser/bin/gpx.xsd');
      let i = 1;
      if (route != "[]") {
        let json2 = JSON.parse(route);
        json2.forEach(async json2 => {
          let inRoute;
          if (json2.name == "") {
            inRoute = 'INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES (\'Unamed route '+(j+1)+'\','+json2.len+','+row.gpx_id+')';
          } else {
            inRoute = 'INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES (\''+json2.name+'\','+json2.len+','+row.gpx_id+')';
          }
          await connection.execute(inRoute);
          j++;
          let num = 'Route '+i+'';
          let way = sharedLib.JSONGetWay(__dirname+'/uploads/'+row.file_name,__dirname+'/parser/bin/gpx.xsd',num);
          if (way != "[]") {
            let k = 0;
            let json3 = JSON.parse(way);
            json3.forEach(async json3 => {
              let inWay;
              if (json3.name == "") {
                inWay = 'INSERT INTO POINT (point_index, latitude, longitude, point_name, route_id) VALUES ('+(k++)+','+json3.latitude+','+json3.longitude+',\'NULL\','+j+')';
              } else {
                inWay = 'INSERT INTO POINT (point_index, latitude, longitude, point_name, route_id) VALUES ('+(k++)+','+json3.latitude+','+json3.longitude+',\''+json3.name+'\','+j+')';
              }
              await connection.execute(inWay);
            })
          }
          i++;
          //let obj = {'comp':"Route "+i+"",'name':json.name,'numPoints':json.numPoints,'len':json.len,'loop':json.loop};
        })
      }
    }


  } catch(e) {
    console.log(e);
    retStr = 0;
  }
  res.send(
    {
      ret: retStr
    }
  );
});

app.get('/clear', async function(req , res){
  
  let retStr = 1;
  try {
    let connection = await mysql.createConnection(dbConf);
    await connection.execute("DELETE FROM FILE");
  } catch(e) {
    retStr = 0;
  }
  console.log(retStr);
  res.send(
    {
      ret: retStr
    }
  );
});

app.get('/status', async function(req , res){
  
  let retStr;
  try {
    let connection = await mysql.createConnection(dbConf);
    const [rows1, fields1] = await connection.execute('SELECT * from `FILE` ORDER BY `gpx_id`');
    console.log(rows1.length);
    const [rows2, fields2] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_id`');
    const [rows3, fields3] = await connection.execute('SELECT * from `POINT` ORDER BY `point_id`');
    let obj = {'file':rows1.length,'route':rows2.length,'point':rows3.length};
    retStr = obj;
  } catch(e) {
    retStr = 0;
  }
  console.log(retStr);
  res.send(
    {
      ret: retStr
    }
  );
});

app.get('/printRoutes', async function(req , res){
  let type = req.query.data1;
  let retStr;
  try {
    let connection = await mysql.createConnection(dbConf);
    const [rows1, fields1] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_name`');
    const [rows2, fields2] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_len`');
    if (type == "name") {
      retStr = rows1;
    } else {
      retStr = rows2;
    }
  } catch(e) {
    console.log(e);
    retStr = 0;
  }
  res.send(
    {
      ret: retStr
    }
  );
});

app.get('/printRoutesID', async function(req , res){
  let retStr;
  try {
    let connection = await mysql.createConnection(dbConf);
    const [rows1, fields1] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_id`');
    retStr = rows1;
  } catch(e) {
    console.log(e);
    retStr = 0;
  }
  res.send(
    {
      ret: retStr
    }
  );
});

app.get('/printSpecRoutes', async function(req , res){
  let File = req.query.data1;
  let type = req.query.data2;
  let objArr = [];
  try {
    let connection = await mysql.createConnection(dbConf);
    const [rows1, fields1] = await connection.execute('SELECT * from `FILE` ORDER BY `gpx_id`');
    const [rows2, fields2] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_name`');
    const [rows3, fields3] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_len`');
    for (let row of rows1) {
      if (row.file_name == File) {
        if (type == "name") {
          for (let row2 of rows2) {
            if (row.gpx_id == row2.gpx_id) {
              let obj = {'route_id':row2.route_id,'route_name':row2.route_name,'route_len':row2.route_len};
              objArr.push(obj);
            }
          }
        } else {
          for (let row3 of rows3) {
            if (row.gpx_id == row3.gpx_id) {
              let obj = {'route_id':row3.route_id,'route_name':row3.route_name,'route_len':row3.route_len};
              objArr.push(obj);
            }
          }
        }
      }
    }
  } catch(e) {
    console.log(e);
    retStr = 0;
  }
  res.send(
    {
      ret: objArr
    }
  );
});

app.get('/printSpecRoutesPoints', async function(req , res){
  let File = req.query.data1;
  let type = req.query.data2;
  let objArr = [];
  try {
    let connection = await mysql.createConnection(dbConf);
    const [rows1, fields1] = await connection.execute('SELECT * from `FILE` ORDER BY `gpx_id`');
    const [rows2, fields2] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_name`');
    const [rows3, fields3] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_len`');
    const [rows4, fields4] = await connection.execute('SELECT * from `POINT` ORDER BY `point_index`');
    for (let row of rows1) {
      if (row.file_name == File) {
        if (type == "name") {
          for (let row2 of rows2) {
            if (row.gpx_id == row2.gpx_id) {
              for (let row4 of rows4) {
                if (row4.route_id == row2.route_id) {
                  let obj = {'route_name':row2.route_name,'point_id':row4.point_id,'point_index':row4.point_index,'latitude':row4.latitude,'longitude':row4.longitude,'point_name':row4.point_name};
                  objArr.push(obj);
                }
              }
            }
          }
        } else {
          for (let row3 of rows3) {
            if (row.gpx_id == row3.gpx_id) {
              for (let row4 of rows4) {
                if (row4.route_id == row3.route_id) {
                  let obj = {'route_name':row3.route_name,'point_id':row4.point_id,'point_index':row4.point_index,'latitude':row4.latitude,'longitude':row4.longitude,'point_name':row4.point_name};
                  objArr.push(obj);
                }
              }
            }
          }
        }
      }
    }
  } catch(e) {
    console.log(e);
    retStr = 0;
  }
  res.send(
    {
      ret: objArr
    }
  );
});

app.get('/printSpecPoints', async function(req , res){
  let route = req.query.data1;
  let objArr = [];
  try {
    let connection = await mysql.createConnection(dbConf);
    const [rows1, fields1] = await connection.execute('SELECT * from `ROUTE` ORDER BY `route_id`');
    const [rows2, fields2] = await connection.execute('SELECT * from `POINT` ORDER BY `point_index`');
    for (let row of rows1) {
      if (row.route_name == route) {
        for (let row2 of rows2) {
          if (row.route_id == row2.route_id) {
            let obj = {'point_id':row2.point_id,'point_index':row2.point_index,'latitude':row2.latitude,'longitude':row2.longitude,'point_name':row2.point_name};
            objArr.push(obj);
          }
        }
      }
    }
  } catch(e) {
    console.log(e);
    retStr = 0;
  }
  res.send(
    {
      ret: objArr
    }
  );
});

app.get('/printSpecShort', async function(req , res){
  let File = req.query.data1;
  let type = req.query.data2;
  let num = req.query.data3;
  let objArr = [];
  try {
    let connection = await mysql.createConnection(dbConf);
    const [rows1, fields1] = await connection.execute('SELECT * from `FILE` ORDER BY `gpx_id`');
    
    for (let row of rows1) {
      if (row.file_name == File) {
        if (type == "short") {
          const [rows2, fields2] = await connection.execute('SELECT * from `ROUTE` WHERE `gpx_id` = '+row.gpx_id+' ORDER BY `route_len` ASC LIMIT '+num+'');
          for (let row2 of rows2) {
            let obj = {'route_id':row2.route_id,'route_name':row2.route_name,'route_len':row2.route_len};
            objArr.push(obj);
          }
        } else {
          const [rows3, fields3] = await connection.execute('SELECT * from `ROUTE` WHERE `gpx_id` = '+row.gpx_id+' ORDER BY `route_len` DESC LIMIT '+num+'');
          for (let row3 of rows3) {
            let obj = {'route_id':row3.route_id,'route_name':row3.route_name,'route_len':row3.route_len};
            objArr.push(obj);
          }
        }
      }
    }
  } catch(e) {
    console.log(e);
  }
  res.send(
    {
      ret: objArr
    }
  );
});

//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
  res.send(
    {
      somethingElse: retStr
    }
  );
});

app.get('/filesIn', function(req , res) { //This gets the files from uploads and returns an object with all the information
  let retStr = fs.readdirSync(__dirname+'/uploads');
  let files = [];
  retStr.forEach(retStr => {
    let str = sharedLib.GPXtoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
    if (str != "{}") {
      let json = JSON.parse(str);
      let obj = {'file':retStr,'version':json.version,'creator':json.creator,'numWaypoints':json.numWaypoints,'numRoutes':json.numRoutes,'numTracks':json.numTracks};
      files.push(obj);
    }
  })
  res.send(
    {
      ret: files
    }
  );
}); //Complete

app.get('/validate', function(req , res){
  let fileStr = req.query.data1;
  let retVal = validateWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
  res.send(
    {
      somethingElse: retStr
    }
  );
});

app.get('/GPXtoJSON', function(req , res){
  let retStr = req.query.data1;
  let tran = sharedLib.GPXtoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
  res.send(
    {
      somethingElse: tran
    }
  );
});

app.get('/RouteAndTrack', function(req , res){ //This gets all the routes and track infomation of a file
  let retStr = req.query.data1;
  let Paths = [];
  let str = sharedLib.RoutetoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
  let i = 1;
  if (str != "[]") {
    let json = JSON.parse(str);
    json.forEach(json => {
      let obj = {'comp':"Route "+i+"",'name':json.name,'numPoints':json.numPoints,'len':json.len,'loop':json.loop};
      Paths.push(obj);
      i++;
    })
  }
  let str2 = sharedLib.TracktoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
  let j = 1;
  if (str2 != "[]") {
    let json2 = JSON.parse(str2);
    json2.forEach(json2 => {
      let obj2 = {'comp':"Track "+j+"",'name':json2.name,'numPoints':json2.numPoints,'len':json2.len,'loop':json2.loop};
      Paths.push(obj2);
      j++;
    })
  }
  res.send(
    {
      ret: Paths
    }
  );
}); //Complete

app.get('/TracktoJSON', function(req , res){
  let retStr = req.query.data1;
  let tran = sharedLib.TracktoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
  res.send(
    {
      somethingElse: tran
    }
  );
});

app.get('/DatatoJSON', function(req , res){
  let retStr = req.query.data1;
  let retStr1 = req.query.data2;
  let tran = sharedLib.ExtratoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',retStr1);
  res.send(
    {
      somethingElse: tran
    }
  );
});

app.get('/rename', function(req , res){
  let retStr = req.query.data1;
  let retStr1 = req.query.data2;
  let retStr2 = req.query.data3;
  let tran = sharedLib.reName(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',retStr1,retStr2);
  res.send(
    {
      somethingElse: tran
    }
  );
});

app.get('/CreateGPX', function(req , res){
  let retStr = req.query.data1;
  let retStr1 = req.query.data2;
  let tran = "{\"version\":1.1,\"creator\":\""+retStr1+"\"}";
  let tran2 = sharedLib.JSONCreateGPXWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',tran);
  res.send(
    {
      somethingElse: tran2
    }
  );
});

app.get('/CreateRoute', function(req , res){
  let retStr = req.query.data1;
  let retStr1 = req.query.data2;
  let tran = sharedLib.JSONCreateRoute(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',retStr1);
  res.send(
    {
      somethingElse: tran
    }
  );
});

app.get('/CreateWaypoint', function(req , res){
  let retStr = req.query.data1;
  let retStr1 = req.query.data2;
  let retStr2 = req.query.data3;
  let tran = sharedLib.JSONCreateWay(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',retStr1,retStr2);
  res.send(
    {
      somethingElse: tran
    }
  );
});

app.get('/Between', function(req , res){

  let retStr = fs.readdirSync(__dirname+'/uploads');
  let files = [];
  let sourceLat = req.query.data1;
  let sourceLong = req.query.data2;
  let destLat = req.query.data3;
  let destLong = req.query.data4;
  let delta = req.query.data5;

  retStr.forEach(retStr => {
    let str = sharedLib.GPXtoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
    if (str != "{}") {
      let json = JSON.parse(str);
      let obj = {'file':retStr,'version':json.version,'creator':json.creator,'numWaypoints':json.numWaypoints,'numRoutes':json.numRoutes,'numTracks':json.numTracks};
      files.push(obj);
    }
  })
  let Paths = [];
  files.forEach(files => {
    let tmp = sharedLib.JSONBetweenRoute(__dirname+'/uploads/'+files.file,__dirname+'/parser/bin/gpx.xsd',sourceLat,sourceLong,destLat,destLong,delta);
    if (tmp != "[]") {
      let json = JSON.parse(tmp);
      json.forEach(json => {
        let obj2 = {'file':files.file,'name':json.name,'numPoints':json.numPoints,'len':json.len,'loop':json.loop};
        Paths.push(obj2)
      })
    }
    let tmp2 = sharedLib.JSONBetweenTrack(__dirname+'/uploads/'+files.file,__dirname+'/parser/bin/gpx.xsd',sourceLat,sourceLong,destLat,destLong,delta);
    if (tmp2 != "[]") {
      let json2 = JSON.parse(tmp2);
      json2.forEach(json2 => {
        let obj3 = {'file':files.file,'name':json2.name,'numPoints':json2.numPoints,'len':json2.len,'loop':json2.loop};
        Paths.push(obj3)
      })
    }
  })
  res.send(
    {
      ret: Paths
    }
  );
});

app.get('/BetweenRoute', function(req , res){
  let retStr = req.query.data1;
  let sourceLat = req.query.data2;
  let sourceLong = req.query.data3;
  let destLat = req.query.data4;
  let destLong = req.query.data5;
  let delta = req.query.data6;
  let tran = sharedLib.JSONBetweenRoute(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',sourceLat,sourceLong,destLat,destLong,delta);
  res.send(
    {
      ret: tran
    }
  );
});

app.get('/BetweenTrack', function(req , res){
  let retStr = req.query.data1;
  let sourceLat = req.query.data2;
  let sourceLong = req.query.data3;
  let destLat = req.query.data4;
  let destLong = req.query.data5;
  let delta = req.query.data6;
  let tran = sharedLib.JSONBetweenTrack(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',sourceLat,sourceLong,destLat,destLong,delta);
  res.send(
    {
      ret: tran
    }
  );
});

app.get('/LengthPath', function(req , res){
  let retStr = fs.readdirSync(__dirname+'/uploads');
  let files = [];
  let length = req.query.data1;
  let delta = req.query.data2;
  retStr.forEach(retStr => {
    let str = sharedLib.GPXtoJSONWrapper(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd');
    if (str != "{}") {
      let json = JSON.parse(str);
      let obj = {'file':retStr,'version':json.version,'creator':json.creator,'numWaypoints':json.numWaypoints,'numRoutes':json.numRoutes,'numTracks':json.numTracks};
      files.push(obj);
    }
  })
  let i = 0;
  let j = 0;
  files.forEach(files => {
    i += sharedLib.RouteLength(__dirname+'/uploads/'+files.file,__dirname+'/parser/bin/gpx.xsd',length,delta);
    j += sharedLib.TrackLength(__dirname+'/uploads/'+files.file,__dirname+'/parser/bin/gpx.xsd',length,delta);
  })
  let count = {'Route':i,'Track':j};
  res.send(
    {
      ret: count
    }
  );
});

app.get('/LengthRoute', function(req , res){
  let retStr = req.query.data1;
  let length = req.query.data2;
  let delta = req.query.data3;
  
  var tran = sharedLib.RouteLength(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',length,delta);
  res.send(
    {
      somethingElse: tran
    }
  );
});

app.get('/LengthTrack', function(req , res){
  let retStr = req.query.data1;
  let length = req.query.data2;
  let delta = req.query.data3;
  
  var tran = sharedLib.TrackLength(__dirname+'/uploads/'+retStr,__dirname+'/parser/bin/gpx.xsd',length,delta);
  res.send(
    {
      somethingElse: tran
    }
  );
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);