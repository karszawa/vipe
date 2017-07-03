import React from 'react';
import styled, { keyframes } from 'styled-components';
import { Row, Input, Button, Card } from 'react-materialize';

const Container = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  justify-content: space-around;
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

const CenteredContainer = styled.div`
  width: 100%;
  display: flex;
  flex-direction: column;
  justify-content: center;
`

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
        <Title className=''>
          <img alt="vipe" src="assets/vipe.svg" width="180"/>
        </Title>

        <Card className='white min-width-260' textClassName='white-text' title='Server mode'>
          <CenteredContainer>
            <Button className='mt-20' waves='light' onClick={ this.onClickServerConnectButton.bind(this) }>ESTABLISH SERVER</Button>
          </CenteredContainer>
        </Card>

        <Card className='white min-width-260' textClassName='white-text' title='Client mode'>
          <CenteredContainer>
            <Input label="Host IP" value={this.state.host_ip} onChange={ (event) => { this.setState({ host_ip: event.target.value}); } } />
            <Input label="Host Port" value={this.state.host_port} onChange={ (event) => { this.setState({ host_port: event.target.value}); } } />
            <Button waves='light' onClick={ this.onClickClientConnectButton.bind(this) }>CONNECT TO SERVER</Button>
          </CenteredContainer>
        </Card>
      </Container>
    );
  }
}
