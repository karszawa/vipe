import React from 'react';
import styled from 'styled-components';
import { Icon, Button, Input, Collection, CollectionItem } from 'react-materialize';

const Container = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  align-items: center;
  background-color: #26a69a;
  padding: 70px 0;
  color: white;
`;

const HostInformation = styled.div`
  display: flex;
  flex-direction: column;
  justify-content: center;
  border-bottom: 1px dotted rgba(0, 0, 0, 0.26);
  width: 60%;
`;

const HostLabel = styled.label`
  text-align: center;
  color: rgba(0, 0, 0, 0.26);
  font-size: .75rem;
`;

const HostAddress = styled.label`
  text-align: center;
  color: rgba(0, 0, 0, 0.26);
  font-size: 1rem;
  line-height: 2.5rem;
`;

const AttendeeListWrapper = styled.ul`
  flex: 1;
`;

const AttendeeListItem = styled.li`

`;

export default class Connecting extends React.Component {
  constructor(props) {
    super(props);
  }

  render() {
    return (
      <Container>
        <Icon className="call-icon">call</Icon>

        <HostInformation>
          <HostLabel>HOST INFORMATION</HostLabel>
          <HostAddress>{ `${this.props.host_ip} : ${this.props.host_port}` }</HostAddress>
        </HostInformation>

        <Button>Disconnect <Icon large right>call_end</Icon></Button>
      </Container>
    );
  }
}
