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

const isProduction = true

app.on('ready', () => {
  mainWindow = new BrowserWindow({ width: (isProduction ? 380 : 1200), height: 700 });
  mainWindow.loadURL(`${rootPath}/index.html`);

  if(!isProduction) {
    mainWindow.webContents.openDevTools();
  }

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
});

app.on("window-all-closed", () => {
  app.quit();
});

app.setName('VIPE');
