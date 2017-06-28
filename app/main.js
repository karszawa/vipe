import app from "app";
import BrowserWindow from "browser-window";
import crashReporter from 'crash-reporter';
import {ipcMain} from "electron";

if(process.env.NODE_ENV === 'develop'){
  crashReporter.start();
}

const rootPath = `file://${__dirname}`;
let mainWindow = null;

app.on('window-all-closed', () => {
  if (process.platform != 'darwin') {
    app.quit();
  }
});

app.on('ready', () => {
  mainWindow = new BrowserWindow({width: 800, height: 600});
  mainWindow.loadURL(`${rootPath}/index.html`);
  mainWindow.webContents.openDevTools();
  mainWindow.on('closed', () => {
    mainWindow = null;
  });
});

app.on("window-all-closed", () => {
  app.quit();
});
