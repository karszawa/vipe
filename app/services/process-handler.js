import { spawn } from 'child_process';

class ProcessHandler {
  constructor(binary, args) {
    this.binary = binary;
    this.args = args;
    this.stdoutHandlers = { };
    this.stderrHandlers = { };
  }

  run() {
    this.process = spawn(`./bin/${this.binary}`, this.args);

    this.process.stdout.on('data', (rawData) => {
      const data = new Buffer(rawData).toString('utf-8');

      data.split('\n').each((line) => {
        console.log(`RECEIVED(stdout): ${line}`);
        this.execHandler('stdout', line);
      });
    });

    this.process.stderr.on('data', (rawData) => {
      const data = new Buffer(rawData).toString('utf-8');
      console.log(`RECEIVED(stderr): ${data}`);

      data.split('\n').forEach((line) => {
        this.execHandler('stderr', line);
      });
    });
  }

  execHandler(std, data) {
    const match_result = data.match(/(.+):\s*(.*)/i);

    if(match_result == null || match_result.length < 1) {
      console.log(`UNMATCHED DATA: ${data}`);
      return;
    }

    const identifier = match_result[1];
    const content = (match_result.length >= 3 ? match_result[2] : '');
    const target_handlers = (std == 'stdout' ? this.stdoutHandlers : this.stderrHandlers);

    if(target_handlers[identifier]) {
      target_handlers[identifier](content);
    } else {
      console.log(`UNKNOWN IDENTIFIER: ${data}`);
    }
  }

  onStdout(key, handler) {
    this.stdoutHandlers[key] = handler;
  }

  onStderr(key, handler) {
    this.stderrHandlers[key] = handler;
  }

  exit() {
    if(this.process) {
      spawn('kill', [ this.process.pid ]);
    }

    return false;
  }
}

export class ServerProcessHandler extends ProcessHandler {
  constructor(udp_port, tcp_port) {
    super('server', [ udp_port, tcp_port ]);
  }
}

export class ClientProcessHandler extends ProcessHandler {
  constructor(host_ip, udp_port, tcp_port) {
    super('client', [ host_ip, udp_port, tcp_port ]);
  }
}
