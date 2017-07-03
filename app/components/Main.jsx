import React from 'react';
import * as ip from 'ip';
import Setup from './Setup.jsx';
import Connecting from './Connecting.jsx';
import Connected from './Connected.jsx';
import styled from 'styled-components';
import Footer from './Footer.jsx';
import { ServerProcessHandler, ClientProcessHandler } from '../services/process-handler.js'
import { getRandomInt } from '../lib/common.js';

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
      error: '',
      is_server: false,
      attendees: [ ]
    };
  }

  componentWillUnmount() {
    console.log('Kill child processes');

    if(this.client_process) {
      this.client_process.exit();
    }

    if(this.server_process) {
      this.server_process.exit();
    }
  }

  connect(host_ip, host_port) {
    this.setState({ scene: SCENES.CONNECTING });

    if(host_ip != '') {
      this.setState({ host_ip: host_ip, host_port: host_port });

      this.client_process = new ClientProcessHandler(host_ip, host_port);

      this.client_process.onStderr('CONNECTED', (val) => {
        this.setState({ scene: SCENES.CONNECTED });
      });

      this.client_process.run();
    } else {
      const port = 54321; // getRandomInt(50000, 59999);

      this.setState({ is_server: true, host_ip: ip.address(), host_port: port });

      this.server_process = new ServerProcessHandler(port);

      this.server_process.onStderr('ESTABLISHED', (val) => {
        setTimeout(() => {
          this.client_process = new ClientProcessHandler(ip.address(), port);

          this.client_process.onStderr('CONNECTED', (val) => {
            this.setState({
              scene: SCENES.CONNECTED,
              attendees: [ ip.address() + ' (you)' ]
            });
          });

          this.client_process.onStderr('CONNECTIONS', (val) => {
            this.setState({ attendees: val.split(',').map((v) => v === ip.address() ? v + ' (you)' : v) });
          });

          this.client_process.run();
        }, 500);
      });

      this.server_process.run();
    }
  }

  disconnect() {
    this.setState({ scene: SCENES.SETUP });

    if(this.client_process) {
      this.client_process.exit();
    }

    if(this.server_process) {
      this.server_process.exit();
    }
  }

  onClick() {
    console.log("onClick");
  }

  render() {
    const content = () => {
      switch(this.state.scene) {
        case SCENES.SETUP: return <Setup onConnect={ this.connect.bind(this) }/>;
        case SCENES.CONNECTED: return (
          <Connected
            host_ip={ this.state.host_ip }
            host_port={ this.state.host_port }
            disconnect={ this.disconnect.bind(this) }
            attendees={ this.state.attendees }
          />
        );
        case SCENES.CONNECTING: return <Connecting />;
        default: return null;
      }
    };

    return (
      <Container>
        { content() }

        <Footer styles="height: 20vh;"/>
        <button onClick={ this.onClick.bind(this) }>val</button>
      </Container>
    );
  }
}
