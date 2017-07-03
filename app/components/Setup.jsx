import React from 'react';
import styled, { keyframes } from 'styled-components';
import { Row, Input, Button } from 'react-materialize';

const Container = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
`;

const appearAnimation = keyframes`
  from { color: transparent; }
  to { color: #26a69a; }
`;

const Title = styled.h1`
  color: #26a69a;
  animation: ${appearAnimation} 3s ease-in;
  font-size: 2.8rem;
`;

export default class Setup extends React.Component {
  constructor(props) {
    super(props);

    this.state = {
      host_ip: '',
      host_port: ''
    };
  }

  onClickClientConnectButton() {
    this.props.onConnect(this.state.host_ip, this.state.host_port);
  }

  onClickServerConnectButton() {
    this.props.onConnect('', '');
  }

  render() {
    return (
      <Container>
        <Title className=''>VIPE</Title>
        <Row>
          <Input s={6} label="Host IP" value={this.state.host_ip} onChange={ (event) => { this.setState({ host_ip: event.target.value}); } } />
          <Input s={6} label="Host Port" value={this.state.host_port} onChange={ (event) => { this.setState({ host_port: event.target.value}); } } />
        </Row>
        <Container>
          <Button waves='light' onClick={ this.onClickClientConnectButton.bind(this) }>CONNECT TO SERVER</Button>
        </Container>
        <Container>
          <Button waves='light' onClick={ this.onClickServerConnectButton.bind(this) }>ESTABLISH SERVER</Button>
        </Container>
      </Container>
    );
  }
}
