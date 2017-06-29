import React from 'react';
import Setup from './Setup.jsx';
import Connecting from './Connecting.jsx';
import Connected from './Connected.jsx';
import styled from 'styled-components';
import { spawn } from 'child_process';
import Footer from './Footer.jsx';

const SCENES = {
  SETUP: 'SETUP',
  CONNECTING: 'CONNECTING',
  CONNECTED: 'CONNECTED'
};

const Container = styled.div`
  width: 100vw;
  height: 95vh;
`;

export default class Main extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      scene: SCENES.SETUP,
      host_ip: '',
      host_port: '',
      error: ''
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

    this.process.stderr.on('data', (rawData) => {
      const data = new Buffer(rawData).toString('utf-8');
      console.log(data);

      if(this.state.error === '') {
        this.setState({ error: data });
      }
    });
  }

  render() {
    const content = () => {
      switch(this.state.scene) {
        case SCENES.SETUP: return <Setup onConnect={ this.onConnect.bind(this) }/>;
        case SCENES.CONNECTED: return <Connected host_ip={ this.state.host_ip } host_port={ this.state.host_port } />;
        case SCENES.CONNECTING: return <Connecting />;
        default: return null;
      }
    };

    return (
      <Container>
        { content() }

        <Footer styles="height: 20vh;"/>
      </Container>
    );
  }
}
