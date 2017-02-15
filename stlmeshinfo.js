var express = require('express');         //Express Web Server 
var busboy = require('connect-busboy');   //middleware for form/file upload
var path = require('path');               //used for file path
var fs = require('fs-extra');             //File System - for file manipulation

var addon = require('bindings')('addon.node') //C++ AddOn for Stl Mesh Analysis

var app = express();
app.use(busboy());
app.use(express.static(path.join(__dirname, 'views')));

app.route('/upload')
    .post(function (req, res, next) {

        var fstream;
        req.pipe(req.busboy);
        req.busboy.on('file', function (fieldname, file, filename) {
            console.log("Uploading: " + filename);
            fstream = fs.createWriteStream(__dirname + filename);
            file.pipe(fstream);
            fstream.on('close', function () {    
                console.log("Upload Finished of " + filename);
                eval('var obj='+addon.StlMeshInfo(__dirname + filename));
                res.render('result.ejs' , {meshName: filename, meshInfo: obj});
                fs.unlink(__dirname + filename);
            });
        });
    });

var server = app.listen(4000, function() {
    console.log('Listening on port %d', server.address().port);
});