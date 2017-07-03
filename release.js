const packager = require('electron-packager');
const config = require('./package.json');

packager({
    dir: './',
    out: '../release',
    name: config.name,
    platform: 'linux',
    arch: 'x64',
    version: '1.4.14',
    icon: './app/assets/vipe.icns',
    'app-bundle-id': 'jp.co.test.test',
    'app-version': config.version,
    'helper-bundle-id': 'jp.co.test.testapp',
    overwrite: true,
    asar: true,
    prune: true,
    ignore: "node_modules/(electron-packager|electron-prebuilt|\.bin)|release\.js",
    'version-string': {
        CompanyName: '会社名',
        FileDescription: 'アプリケーションの説明',
        OriginalFilename: config.name,
        FileVersion: config.version,
        ProductVersion: config.version,
        ProductName: config.name,
        InternalName: config.name
    }
}, function done (err, appPath) {
    if(err) {
        throw new Error(err);
    }
    console.log('Done!!');
});
