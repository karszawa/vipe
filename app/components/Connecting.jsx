import React from 'react';
import styled from 'styled-components';
import { Preloader } from 'react-materialize';

const Container = styled.div`
  width: 100%;
  height: 100%;
  display: flex;
  justify-content: center;
  align-items: center;
`;

export default class Connecting extends React.Component {
  constructor(props) {
    super(props);
  }

  render() {
    return (
      <Container>
        <Preloader size='big'/>
      </Container>
    );
  }
}
