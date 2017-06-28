import React from 'react';
import Setup from './Setup.jsx';
import Connecting from './Connecting.jsx';
import Connected from './Connected.jsx';
import styled from 'styled-components';
import { spawn } from 'child_process';

const SCENES = {
  SETUP: 'SETUP',
  CONNECTING: 'CONNECTING',
  CONNECTED: 'CONNECTED'
};

const Container = styled.div`
  width: 100vw;
  height: 100vh;
`;

export default class Main extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      scene: SCENES.SETUP,
      host_ip: '',
      host_port: ''
    };
  }

  onConnect(host_ip, host_port) {
    console.log('TRY CONNECTING');

    this.setState({
      scene: SCENES.CONNECTING,
      host_ip: host_ip,
      host_port: host_port
    });

    this.process = spawn('./a.out', [ host_ip, host_port ]);

    this.process.stdout.on('data', (rawData) => {
      const data = new Buffer(rawData).toString('utf-8');
      console.log(data);
      this.setState({ scene: SCENES.CONNECTED });
    });
  }

  render() {
    switch(this.state.scene) {
      case SCENES.SETUP: return <Container><Setup onConnect={ this.onConnect.bind(this) }/></Container>;
      case SCENES.CONNECTED: return <Container><Connected host_ip={ this.state.host_ip } host_port={ this.state.host_port } /></Container>;
      case SCENES.CONNECTING: return <Container><Connecting /></Container>;
      default: return null;
    }
  }
}
