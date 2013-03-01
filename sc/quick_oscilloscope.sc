QuickOscilloscope : QuickGuiElement {
	var <server, m_bus, synth;
	var <maxCycle, <cycle;

	*new { arg gui, path, bus, maxCycle = 4096, server;
		if (server.isNil) { server = Server.default };
		if (bus.isNil) { bus = Bus(server:server) };
		^super.new(gui,path).init(server ? Server.default, bus, maxCycle);
	}

	init { arg server_, bus_, maxCycle_;
		server = server_;
		m_bus = bus_;
		synth = BusScopeSynth(server);
		maxCycle = maxCycle_;
		this.cycle_(1024);
	}

	cycle_ { arg frames;
		cycle = min(maxCycle, frames);
		synth.setCycle(cycle);
	}

	start {
		if (server.serverRunning.not) {
			warn("QuickOscilloscope: Can not start - server not running.");
			^this;
		};

		synth.play(maxCycle, m_bus, cycle);

		ServerTree.add(this, server);
		ServerQuit.add(this, server);

		this.invoke(\stop); // make sure we can apply the following settings
		this.set( \server, server.addr.port );
		this.set( \buffer, synth.bufferIndex );
		this.invoke(\start);
	}

	stop {
		synth.stop;

		ServerTree.remove(this, server);
		ServerQuit.remove(this, server);

		this.invoke(\stop);
	}

	bus {
		^m_bus.copy;
	}

	bus_ { arg bus;
		// have to restart
		var was_playing = synth.isPlaying;
		synth.stop;
		m_bus = bus;
		if (was_playing) { this.start };
	}

	doOnServerTree {
		this.start;
	}

	doOnServerQuit {
		synth.stop;
	}
}
