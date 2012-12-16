QuickGui
{
	classvar allGuis;
	var <serverAddress, <port, subscriptions;

	*new { arg serverAddress;
		var me;
		var port = NetAddr.langPort; // FIXME: allow arbitrary port
		allGuis.do { |gui|
			if (gui.serverAddress == serverAddress and: (gui.port == port)) { ^gui };
		};
		me = super.newCopyArgs(serverAddress);
		allGuis = allGuis.add(me);
		^me;
	}

	subscribe { arg path ...signalFuncArray;
		var signals;
		if(subscriptions.isNil) { subscriptions = IdentityDictionary() };
		signalFuncArray.pairsDo { |signal, func|
			var fullPath = (path ++ $/ ++ signal).asSymbol;
            var listener = subscriptions[fullPath];
			var wrapFunc = { |msg| func.valueArray(msg) };
            if (listener.isNil) {
				listener = OSCFunc(
					wrapFunc,
					fullPath,
					recvPort: port
				).permanent_(true);
                subscriptions[fullPath] = listener;
            }{
				// FIXME: OSCFunc:-func should really be settable:
				listener.prFunc_(wrapFunc);
            };
			signals = signals.add(signal);
        };
		serverAddress.sendMsg('/subscribe', path, *signals);
	}

    unsubscribe { arg path ...signals;
        signals.do { |signals|
			var fullPath = (path ++ $/ ++ signals).asSymbol;
			var listener = subscriptions[fullPath];
			if (listener.notNil) { listener.free };
            subscriptions[fullPath] = nil;
        };
        serverAddress.sendMsg('/unsubscribe', path, *signals);
    }

	unsubscribeAll { arg path;
		var signalPattern = path ++ $/;
		subscriptions.keys.do { |signal|
			var listener;
			if (signal.asString.beginsWith(signalPattern)) {
				listener = subscriptions[signal];
				if (listener.notNil) { listener.free };
				subscriptions[signal] = nil;
			};
		};
		serverAddress.sendMsg('/unsubscribeAll', path);
	}

	sendMsg { arg path ...arguments;
		serverAddress.sendMsg(path, *arguments);
	}
}

QuickGuiElement
{
    var gui, path, setPath, invokePath;

    *new { arg gui, path;
        ^super.newCopyArgs(
			gui,
			path,
			("/set" ++ path).asSymbol,
			("/invoke" ++ path).asSymbol
		);
    }

    subscribe { arg ...args;
		gui.subscribe(path, *args);
    }

    unsubscribe { arg ...signals;
		gui.unsubscribe(path, *signals);
    }

	unsubscribeAll {
		gui.unsubscribeAll(path);
	}

    set { arg property, value;
        gui.sendMsg(setPath, property, value);
    }

	invoke { arg method ...arguments;
		gui.sendMsg(invokePath, method, *arguments);
	}
}
