import React from 'react';
import styled from 'styled-components';
import * as ip from 'ip';

const Footer = styled.footer`
  display: flex;
  justify-content: center;
  align-items: center;
  height: 5vh;
  background-color: #38435a;
`;

const Text = styled.p`
  color: rgba(255, 255, 255, 0.65);
  font-size: 12px;
`

export default class OriginalFooter extends React.Component {
  constructor(props) {
    super(props);

    this.state = { address: ip.address() };
  }

  render() {
    return (
      <Footer>
        <Text>Your IP Address: { this.state.address }</Text>
      </Footer>
    );
  }
}
