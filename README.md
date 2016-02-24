# react_test

Electron+Reactなサンプルアプリ by サーバサイドエンジニア

## 概要

JavascriptはES6以前、ES6(ECMAScript2015)で書き方が違うので、Electronを紹介するサイトによって結構差異があります。
サーバーサイドばかりでこのあたり詳しくなく、どっちが新しいの？と混乱しながらサンプルを作っていました。。。
また、Reactも頻繁に新しい機能が追加されるので、ReactDOMを使わずに書いてる古い形式も混在してたりカオスな状態です。

なので現時点(2015/02/24)で、調べた限りで新しいスタイルで記述してみました

フロントエンド技術に詳しくないため、間違った記述等あればプルリク・コメントお待ちしています！

### アプリの機能 

HelloWorld的な機能のみです.

* Reactを使った画面表示
* ボタンクリックを認識して、コンソールログにクリックカウントを出力
* デバッグ用にデベロッパーツールを表示
* babel-registerによるjsxの自動変換
* ESLintによる検証

何故か動かない！という時に便利なデベロッパーツールを表示させたり、クリック制御ってどうやるの？的な事を知りたかったので入れています

### 使い方

```bash
cd repository_dir
clone https://github.com/kegamin/electron_test.git electron_test
cd electron_test
npm install
npm start
```

### やらなかった点

最終的には以下も試したいが、レポジトリが複雑になると参考にしずらいので今回はやっていません

* bowerを利用したフロントエンドの管理(bootstrapとか)
* reduxを使ったMV*的な作り込み
* react-route等のReactの色々な機能
* gulpを利用した自動化

### リポジトリ構成

```bash
.
├── .eslintrc         # ESLint設定ファイル
├── app               #
│   ├── index.html    # Electron メイン画面
│   ├── main.js       # Electronアプリ本体
│   ├── script        #
│   │   ├── click.jsx # ボタンクリック時の制御を記載したスクリプト
│   │   └── react.jsx # Reactの制御を記載したスクリプト
│   └── view          #
│       └── view.jsx  # ReactのViewファイル
├── index.js          # Electron起動時に呼ばれるファイル(package.jsonで指定)
└── package.json      # Node.js(Electron)設定ファイル
```

以下のような流れで処理が進んでいき、最終的に画面表示がされます.  
(ちょっと語弊がありますが、ファイルの読む順番として理解してもらえれば)

```
package.json -> index.js -> main.js -> index.html -> (react.jsx + view.jsx)
                                           click! -> click.jsx
```

## ES6/React用に考慮・変更した点

以下本アプリを作りにあたって、参考サイトから変更したりした点です.
* require -> importに統一
* varからconst/letに変更
* babel-registerを利用したJSX->JS変換
* 文字列中の変数をテンプレートリテラルに
* ReactDOMを利用する
* React.Componentを継承したクラス

### require -> importに統一

色んなサイトで混在してたけど、ES6の形式であるimportに統一しています

参考：[Babelで理解するEcmaScript6の import / export](http://qiita.com/inuscript/items/41168a50904242005271)

```javascript
const App = require('App');
const ipcMain = require('electron').ipcMain;

↓変更

import app from "app";
import {ipcMain} from "electron";
```

### varからconst/letに変更

varでサンプル作ってる人も居ましたが、ES6ではconst/letを使う方がいいですね。varは怖い…

[ES6のconstを使い倒すレシピ1 - 前提共有編 〜 JSおくのほそ道 #034](http://qiita.com/hosomichi/items/093aadcd8891c86e9dda)

```javascript
const myButton = document.getElementById("Click");

let clickCount = 0;
```

### babel-registerを利用したJSX->JS変換

Reactで使用するjsxファイルは、最終的にjsファイルに変換してElectronが読める状態にする必要があります.
Gulpを利用しているサイトもありましたが、babel-registerの方がカジュアルだったので今回採用しています.

以下のコードをindex.jsとindex.htmlに入れています.

```javascript
require('babel-register');
```

ただしこのままで実行時にエラーになります。以下のようなエラーしか吐かないので、原因わかるまで苦労しました…

```
Uncaught Exception: SyntaxError: Unexpected token import....
```

これはpackage.jsonで以下の指定をしてES6及びReact形式をbabelに認識させる必要があります。

```javascript
  "babel": {
    "sourceMaps": "inline",
    "presets": [
      "es2015",
      "react"
    ]
  },
```

### 文字列中の変数をテンプレートリテラルに

ES6で使えるようになった機能。rubyとかでやる`"#{hoge}"`と同じ形式が使えるようになったので合わせました。

参考：[JavaScript の テンプレートリテラル を極める!](http://qiita.com/kura07/items/c9fa858870ad56dfec12)

```javascript
mainWindow.loadURL(rootPath + "/index.html");

↓変更

mainWindow.loadURL(`${rootPath}/index.html`);
```

### ReactDOMを利用する

Reactは途中からDOMを制御する部分を分離し、ReactDOMを使うようになりました

```javascript
import React from 'react';

React.render(
  <Index />,
  document.getElementById('content')
);

↓変更

import React from 'react';
import ReactDOM from 'react-dom';

ReactDOM.render(
  <Main />,
  document.getElementById('content')
);
```

### React.Componentを継承したクラス

ES6なのでReact.Componentを継承したクラスを作って利用できるようになりました。
以下は`app/view/view.jsx`のソースコードですが、一番いろんなサイト参考にしたかも.

メソッド追加して色々やるのに、こっちの方が断然拡張性・保守性がいいですね.つか見やすい

```javascript
import React from 'react';

export default class Main extends React.Component {
  constructor(props) {
    super(props);
    this.state = { message: "hello message from react" };
  }
  return (
    <div>{this.state.message}</div>
  );
}
```

## ESLint

Javascriptの静的検証ツールを入れて、逐次問題ないか確認しながらやりました。
ここもReact+Electronで使うにあたって情報が少なく結構困った…

参考：[ESLint 最初の一歩](http://qiita.com/mysticatea/items/f523dab04a25f617c87d)

最低限？しか入れてないですが、おおよそ以下の設定でエラー無くなりました。

```json
{
  "extends": "eslint:recommended",
  "parser": "babel-eslint",
  "react/jsx-uses-react": 1,
  "ecmaFeatures" : {
    "jsx": true,
    "objectLiteralShorthandMethods" : true
  },
  "plugins": [
    "react"
  ],
  "rules": {
    "strict": 0,
    "indent": [ 2, 2],
    "semi": [2, "always"],
    "no-console": 0
  },
  "env": {
    "es6": true,
    "node": true,
    "browser": true
  }
}
```

### ※※※ is defined but never used ワーニング

以下のコード部分で、`Main`部分はESLintでワーニングが発生します。
原因は`no-unused-vars`のチェックが働いているためで、宣言してるのに使ってないからというメッセージです。

```javascript
import Main from '../view/view.jsx';

ReactDOM.render(
  <Main />,
  document.getElementById('content')
);
```

これは現状どうしようも無いため一旦放置.一応無理やり回避はリンクにありますが…

> No. ESLint supports JSX syntax as defined in the spec. We don't specifically support React, just like we don't specifically support jQuery.
>
> [es6jsx: no-unused-vars misses implicit React variable usage in JSX #1867](https://github.com/eslint/eslint/issues/1867)

## 補足

### IntelliJでElectron開発

一緒にIntelliJの設定でも苦労したので、qiitaに登録しています

[IntelliJ(or WebStorm)でElectron開発環境を準備する](http://qiita.com/kegamin/items/d92df454e2978968d36d)

### 次にやること

以下を追加してもうちょっとらしいアプリにする！

* redux
* react-route
* bower
* bootstrap
* gulp

### 参考サイト

もっとありましたが、一番参考にしたサイトは以下の通り

* [Electron + React + Redux](http://tech.kayac.com/archive/which-would-you-like-pancake-or-sandwich.html)
* [ぼくのかんがえたさいきょうのElectron](http://qiita.com/Quramy/items/90d61ff37ca1b95a7f6d)
* [chentsulin/electron-react-boilerplate](https://github.com/chentsulin/electron-react-boilerplate)
* [b52/electron-es6-react](https://github.com/b52/electron-es6-react)
* kunikenさんの一連のqiita投稿
 * [今からはじめるReact.js〜準備〜](http://qiita.com/kuniken/items/19fccc27b8bfa8068d22)
 * [今からはじめるReact.js〜初めてのコンポーネント〜](http://qiita.com/kuniken/items/963cb977dffd3e662e40)
 * [今からはじめるReact.js〜コンポーネントをまとめる〜](http://qiita.com/kuniken/items/c0f5ed06695ce52d4854)
 * [今からはじめるReact.js〜スタイルの適用〜](http://qiita.com/kuniken/items/b06de893c22f33499a22)
 * [今からはじめるReact.js〜React ver0.14〜](http://qiita.com/kuniken/items/2fc5b782da5d302247ab)
 * [今からはじめるReact.js〜propsとstate、それからrefs〜](http://qiita.com/kuniken/items/a22adda392ccc30011b1)
