/*

ESP8266 file system builder with PlatformIO support

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// -----------------------------------------------------------------------------
// File system builder
// -----------------------------------------------------------------------------

// this file is taken from
// http://tinkerman.cat/optimizing-files-for-spiffs-with-gulp/
// and adapted to OmniESP layout
// we adapted it also to use gulp 4 to make sure clean has terminated
// before anything else is written to the webdir

// install
// del gulp@4 gulp-clean-css gulp-gzip gulp-htmlmin gulp-if gulp-inline gulp-plumber gulp-uglify gulp-useref gulp-util yargs git-tools
// with npm install here or install them globally with npm install -q and
// link them with npm lin

"use strict";

const htmldir = "html";
const datadir = "data";
const bindir = ".pioenvs/d1_mini";
const updatedir = "update";
const webdir = datadir + "/web";
const customDeviceh = "src/customDevice/customDevice.h";

const gulp = require('gulp');
const plumber = require('gulp-plumber');
const htmlmin = require('gulp-htmlmin');
const cleancss = require('gulp-clean-css');
const uglify = require('gulp-uglify');
const gzip = require('gulp-gzip');
const del = require('del');
const useref = require('gulp-useref');
const gulpif = require('gulp-if');
const inline = require('gulp-inline');
const tar = require('gulp-tar-path');
const rename = require('gulp-rename');
const fs = require('fs');
//const {gitDescribe, gitDescribeSync} = require('git-describe');
const Repo = require('git-tools');
//const preprocess = require('gulp-preprocess');

//        .pipe(preprocess({context: { NODE_PATH: '$NODE_PATH:node_modules'}}))

var branch, description, devicetype, version;
var vobj= {}; // version object

/* versioning */
gulp.task('getBranch', function() {
  return new Promise(function(resolve, reject) {
    var repo = new Repo(".");
    repo.currentBranch(function(err, value) {
      if (err) throw err;
      branch = value;
      resolve();
    });
  });
});

gulp.task('getDescription', function() {
  return new Promise(function(resolve, reject) {
    var repo = new Repo(".");
    repo.describe({
      long: true,
      tags: true
    }, function(err, value) {
      if (err) throw err;
      description = value;
      resolve();
    });
  });
});

gulp.task('getDevicetype', function() {
  return new Promise(function(resolve, reject) {
    // give encoding, else a buffer is returned
    fs.readFile(customDeviceh, "utf8", function(err, data) {
      if (err) throw err;
      // console.log(data); // prints whole file
      var result= data.match(/#define\s+DEVICETYPE\s+\"(.+)\"/);
      if (result) {
        devicetype = result[1]; // 0: whole match, 1: parentheses
        resolve();
      } else {
        throw Error("DEVICETYPE not found in " + customDeviceh);
        reject();
      }
    });
    resolve();
  });
});

gulp.task('showVersion', function() {
  return new Promise(function(resolve, reject) {
    version = devicetype + "-" + branch + "-" + description;
    vobj.version= version;
    vobj.date= new Date();
    vobj.devicetype= devicetype;
    vobj.branch= branch;
    vobj.description= description;
    console.log(vobj);
    fs.writeFileSync(datadir + '/version.json', JSON.stringify(vobj), "utf8");
    resolve();
  });
});


gulp.task('versioninfo',
  gulp.series(
    gulp.parallel('getBranch', 'getDescription', 'getDevicetype'),
    'showVersion'
  )
);

/* Clean destination folder */
gulp.task('clean', function() {
  return del([webdir + '/*']);
});


/* Copy static files */
gulp.task('files', function() {
  return gulp.src(htmldir + '/**/*.{jpg,jpeg,png,ico,gif}')
    .pipe(gulp.dest(webdir + '/'));
});

gulp.task('lib', function() {
  return gulp.src(htmldir + '/lib/**/*.*')
    .pipe(gzip())
    .pipe(gulp.dest(webdir + '/lib/'));
});

/* Process HTML, CSS, JS  --- INLINE --- */
gulp.task('inline', function() {
  return gulp.src(htmldir + '/*.html')
    .pipe(inline({
      base: htmldir + '/',
      js: uglify,
      css: cleancss,
      disabledTypes: ['svg', 'img']
    }))
    .pipe(htmlmin({
      collapseWhitespace: true,
      removeComments: true,
      minifyCSS: true,
      minifyJS: true
    }))
    //.pipe(gzip()) // if we gzip, the subst in the webserver does not work
    .pipe(gulp.dest(webdir));
});

/* Process HTML, CSS, JS */
gulp.task('html', function() {
  return gulp.src(htmldir + '/*.html')
    .pipe(useref())
    .pipe(plumber())
    .pipe(gulpif('*.css', cleancss().pipe(gzip())))
    .pipe(gulpif('*.js', uglify()))
    .pipe(gulpif('*.html', htmlmin({
      collapseWhitespace: true,
      removeComments: true,
      minifyCSS: true,
      minifyJS: true
    })))
    //.pipe(gzip()) // if we gzip, the subst in the webserver does not work
    .pipe(gulp.dest(webdir));
});

/* copy firmware to folder structure */
gulp.task('copyfirmware', function() {
  return gulp.src(bindir + '/firmware.bin')
    .pipe(gulp.dest(datadir + '/firmware'));
});

/* delete firmware from folder structure */
// npm install --save-dev gulp del
gulp.task('delfirmware', function() {
  return del(datadir + '/firmware/firmware.bin');
});

/* create firmware update tarball */
// requires:
// npm install --save-dev gulp-tar-path
// see https://www.npmjs.com/package/gulp-tar-path
gulp.task('tar', function() {
  process.chdir(datadir);
  var result = gulp.src(['version.json',
      'firmware/*.bin',
      'web',
      'customDevice/*.json'
    ])
    .pipe(tar(version+'.tar'))
    .pipe(gulp.dest(updatedir));
  process.chdir(__dirname);
  return result;
});

/* Build file system */
// https://fettblog.eu/gulp-4-parallel-and-series/
gulp.task('cleanfs', gulp.series('clean'));
gulp.task('buildfs', gulp.series('clean', 'versioninfo',
  gulp.parallel('files', 'lib', 'html')));
gulp.task('buildfs2', gulp.series('clean', 'versioninfo',
  gulp.parallel('files', 'lib', 'inline')));
gulp.task('default', gulp.series('buildfs', 'versioninfo'));
gulp.task('tarball', gulp.series('buildfs', 'copyfirmware', 'versioninfo', 'tar', 'delfirmware'));

// -----------------------------------------------------------------------------
// PlatformIO support
// -----------------------------------------------------------------------------

const spawn = require('child_process').spawn;
const argv = require('yargs').argv;

var platformio = function(target) {
  var args = ['run'];
  if ("e" in argv) {
    args.push('-e');
    args.push(argv.e);
  }
  if ("p" in argv) {
    args.push('--upload-port');
    args.push(argv.p);
  }
  if (target) {
    args.push('-t');
    args.push(target);
  }
  const cmd = spawn('platformio', args);
  cmd.stdout.on('data', function(data) {
    console.log(data.toString().trim());
  });
  cmd.stderr.on('data', function(data) {
    console.log(data.toString().trim());
  });
};

gulp.task('uploadfs', gulp.series('buildfs'), function() {
  platformio('uploadfs');
});
gulp.task('upload', function() {
  platformio('upload');
});
gulp.task('run', function() {
  platformio(false);
});
