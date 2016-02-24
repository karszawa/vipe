import {ipcRenderer} from "electron";

const myButton = document.getElementById("Click");

let clickCount = 0;

myButton.addEventListener("click", () => {
  ipcRenderer.send("click-my-button", ++clickCount);
});