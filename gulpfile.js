/*

ESP8266 file system builder with PlatformIO support

*/

// -----------------------------------------------------------------------------
// File system builder
// -----------------------------------------------------------------------------

// this file is taken from
// http://tinkerman.cat/optimizing-files-for-spiffs-with-gulp/
// and adapted and greatly enhanced to OmniESP layout
// we adapted it also to use gulp 4 to make sure clean has terminated
// before anything else is written to the webdir

// npm install del gulp@4 gulp-clean-css gulp-gzip gulp-htmlmin gulp-if gulp-inline gulp-plumber gulp-uglify gulp-useref gulp-util yargs git-tools gulp-tar-path gulp-rename

"use strict";

const htmldir = "html";
const datadir = "data";
const bindir = ".pio/build/d1_mini";
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
const Repo = require('git-tools');

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

gulp.task('writeVersionFile', function() {
  return new Promise(function(resolve, reject) {
    version = /*devicetype + "-" +*/ branch + "-" + description;
    vobj.version= version;
    vobj.date= new Date();
    //vobj.devicetype= devicetype;
    vobj.branch= branch;
    vobj.description= description;
    console.log(vobj);
    fs.writeFileSync(datadir + '/version.json', JSON.stringify(vobj), "utf8");
    resolve();
  });
});


gulp.task('versioninfo',
  gulp.series(
    gulp.parallel('getBranch', 'getDescription'/*, 'getDevicetype'*/),
    'writeVersionFile'
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

// lib removed -- all js files will be merged into one
// gulp.task('lib', function() {
//   return gulp.src(htmldir + '/lib/**/*.*')
//     .pipe(gzip())
//     .pipe(gulp.dest(webdir + '/lib/'));
// });


gulp.task('lib', function(done) {
  // stub
  done();
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
    .pipe(gulp.dest(webdir))
});

/* Process HTML, CSS, JS */
gulp.task('html', function() {
  return gulp.src(htmldir + '/*.html')
    .pipe(useref())
    .pipe(plumber())
    .pipe(gulpif('*.css', cleancss().pipe(gzip({
      gzipOptions:{level:9}})))
    )
    .pipe(gulpif('*.js', uglify()))
    .pipe(gulpif('*.html', htmlmin({
      collapseWhitespace: true,
      removeComments: true,
      minifyCSS: true,
      minifyJS: true
    })))
    //.pipe(gzip()) // if we gzip, the subst in the webserver does not work
    .pipe(gulpif('*.js', gzip({gzipOptions:{level:9}})))
    .pipe(gulp.dest(webdir))
    ;
});

/* copy firmware to folder structure */
gulp.task('copyfirmware', function() {
  return gulp.src(bindir + '/firmware.bin')
    .pipe(gulp.dest(datadir + '/firmware'));
});

/* copy firmware to update folder */
gulp.task('cleanparts', function() {
  return del([updatedir + '/*']);
});

gulp.task('copyparts', function() {
  return gulp.src([bindir + '/firmware.bin', bindir + '/spiffs.bin'])
    .pipe(gulp.dest(updatedir));
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
    .pipe(tar(devicetype+"-"+branch+"-"+description+'.tar'))
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
gulp.task('default', gulp.series('buildfs'));
gulp.task('tarball', gulp.series('cleanparts', 'copyparts', 'copyfirmware',
    gulp.parallel('getBranch', 'getDescription', 'getDevicetype'),
   'tar', 'delfirmware'));

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
