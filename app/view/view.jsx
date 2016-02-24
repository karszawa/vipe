import React from 'react';

export default class Main extends React.Component {
  constructor(props) {
    super(props);
    this.state = { message: "hello message from react" };
  }
  render() {
    return (
      <div>{this.state.message}</div>
    );
  }
}
