/**
 * Copyright (C) 2015 GraphKit, Inc. <http://graphkit.io> and Daniel Dahan <daniel@graphkit.io>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program located at the root of the software package
 * in a file called LICENSE.  If not, see <http://www.gnu.org/licenses/>.
 */

'use strict';
'use esnext';

let graphkit = require('../build/Release/graphkit');
const NODE = graphkit.NODE;
const ENTITY = graphkit.ENTITY;
const ACTION = graphkit.ACTION;
const BOND = graphkit.BOND;
let Entity = graphkit.Entity;
let Action = graphkit.Action;
let Bond = graphkit.Bond;
let Index = graphkit.Index;
let Cluster = graphkit.Cluster;
let Graph = graphkit.Graph;

// Entity tests
(function() {
	let e;
	try {
		e = new Entity();
		console.log('Entity empty initialization test failed');
	} catch (error) {}

	try {
		e = new Entity(1);
		console.log('Entity numeric initialization test failed');
	} catch (error) {}

	e = new Entity('User');

	if (!(e instanceof Entity)) {
		console.log('Entity instanceof test failed');
	}


	if (ENTITY != e.nodeClass) {
		console.log('Entity nodeClass test failed');
	}

	if ('User' != e.type) {
		console.log('Entity type test failed');
	}

	if (0 != e.id) {
		console.log('Entity id test failed');
	}

	if (!e.addGroup('admin')) {
		console.log('Entity addGroup("admin") test failed');
	}

	if (!e.addGroup('female')) {
		console.log('Entity addGroup("female") test failed');
	}

	if (2 != e.groupSize()) {
		console.log('Entity groupSize() test failed');
	}

	if (!e.hasGroup('admin')) {
		console.log('Entity hasGroup("admin") test failed');
	}

	if (!e.hasGroup('female')) {
		console.log('Entity hasGroup("female") test failed');
	}

	//if (!e.removeGroup('admin')) {
	//	console.log('Entity removeGroup("admin") test failed');
	//}

	//if (!e.removeGroup('female')) {
	//	console.log('Entity removeGroup("female") test failed');
	//}

	//if (0 != e.groupSize()) {
	//	console.log('Entity groupSize() test failed');
	//}

	e['name'] = 'Eve';
	e['age'] = 26;

	if ('Eve' != e['name']) {
		console.log('Entity insert e["name"] test failed');
	}

	if (26 != e['age']) {
		console.log('Entity insert e["age"] test failed');
	}

	delete e['name'];
	delete e['age'];

	if ('Eve' == e['name']) {
		console.log('Entity remove e["name"] test failed');
	}

	if (26 == e['age']) {
		console.log('Entity remove e["age"] test failed');
	}

	console.log('Entity tests passed');
})();

// Action tests
(function() {
	let a;
	try {
		a = new Action();
		console.log('Action empty initialization test failed');
	} catch (error) {}

	try {
		a = new Action(1);
		console.log('Action numeric initialization test failed');
	} catch (error) {}

	a = new Action('User');

	if (!(a instanceof Action)) {
		console.log('Action instanceof test failed');
	}

	if (ACTION != a.nodeClass) {
		console.log('Action nodeClass test failed');
	}

	if ('User' != a.type) {
		console.log('Action type test failed');
	}

	if (0 != a.id) {
		console.log('Action id test failed');
	}

	console.log('Action tests passed');
})();

// Bond tests
(function() {
	let b;
	try {
		b = new Bond();
		console.log('Bond empty initialization test failed');
	} catch (error) {}

	try {
		b = new Bond(1);
		console.log('Bond numeric initialization test failed');
	} catch (error) {}

	b = new Bond('User');

	if (!(b instanceof Bond)) {
		console.log('Bond instanceof test failed');
	}

	if (BOND != b.nodeClass) {
		console.log('Bond nodeClass test failed');
	}

	if ('User' != b.type) {
		console.log('Bond type test failed');
	}

	if (0 != b.id) {
		console.log('Bond id test failed');
	}

	console.log('Bond tests passed');
})();

// Index tests
(function() {
	let i1 = new Index(ENTITY, 'User');
	let e1 = new Entity('User');
	let e2 = new Entity('Book');
	let a1 = new Action('Read');

	if (ENTITY != i1.nodeClass) {
		console.log('Index nodeClass test failed - got', i1.nodeClassToString);
	}

	if ('User' != i1.type) {
		console.log('Index type test failed');
	}

	if (!i1.insert(e1) || 1 != e1.id || !e1.indexed) {
		console.log('Index insert(e1) test failed');
	}

	try {
		i1.insert(e2);
		console.log('Index insert(e2) test failed');
	} catch (error) {}

	try {
		i1.insert(a1);
		console.log('Index insert(a1) test failed');
	} catch (error) {}

	let count = 1000000;
	let start = Date.now();
	for (let i = count; 0 < i; --i) {
		if (!i1.insert(new Entity('User'))) {
			console.log('Index insert(new Entity("User")) test failed');
			break;
		}
	}
	if (count + 1 != i1.size()) {
		console.log('Index size() test failed, Count', count + 1, 'Size', i1.size());
	}
	console.log('Index insert() tests passed', 'Operations', count, 'Time', Date.now() - start);

	if (!i1.remove(2) || !i1.remove(e1) || count - 1 != i1.size()) {
		console.log('Index remove() test failed');
	}

	for (let i = i1.size() - 1; 0 <= i; --i) {
		if (!i1[i].id) {
			onsole.log('Index iterator[] test failed');
			break;
		}
		if (i == 0) {
			console.log('Index iterator[] test passed');
		}
	}

	if (3 != i1.find(3).id) {
		onsole.log('Index find(3) test failed');
	}

	i1.clear();
	if (0 != i1.size()) {
		console.log('Index clear() test failed, Count', 0, 'Size', i1.size());
	}
})();

// Cluster tests
(function() {
	let c1 = new Cluster(ENTITY);
	let e1 = new Entity('User');
	let e2 = new Entity('Book');

	if (ENTITY != c1.nodeClass) {
		console.log('Cluster nodeClass test failed - got', c1.nodeClassToString);
	}

	if (0 != c1.size()) {
		console.log('Cluster size() test failed, Count', 0, 'Size', c1.size());
	}

	if (!c1.insert(e1) || 1 != e1.id) {
		console.log('Cluster insert(e1) test failed');
	}

	if (!c1.insert(e2) || 1 != e2.id) {
		console.log('Cluster insert(e2) test failed');
	}
	console.log(c1);
	if (1 != c1.User.find(1).id) {
		onsole.log('Cluster find("User", 1) test failed');
	}

	if (1 != c1.find('Book', 1).id) {
		onsole.log('Cluster find("Book", 1) test failed');
	}

	if (!c1.remove(e1.type, e1.id) || !c1.remove(e2)) {
		console.log('Cluster remove() test failed');
	}

	let count = 1000000;
	let start = Date.now();
	for (let i = count; 0 < i; --i) {
		c1.insert(0 == i % 2 ? new Entity('User') : new Entity('Book'));
	}
	console.log('Cluster insert() tests passed', 'Operations', count, 'Time', Date.now() - start);

	for (let i = c1.size() - 1; 0 <= i; --i) {
		if (500000 != c1[i].size()) {
			console.log('Cluster iterator[] test failed');
			break;
		}
		if (i == 0) {
			console.log('Cluster iterator[] test passed');
		}
	}

	c1.clear();
	if (0 != c1.size()) {
		console.log('Cluster clear() test failed, Count', 0, 'Size', c1.size());
	}

	console.log('Cluster tests passed');
})();

// Graph tests
(function() {
	let g1 = new Graph();
	let e1 = new Entity('User');
	let e2 = new Entity('Book');

	if (0 != g1.size()) {
		console.log('Graph size() test failed, Count', 0, 'Size', g1.size());
	}

	if (!g1.insert(e1) || 1 != e1.id) {
		console.log('Graph insert(e1) test failed');
	}

	if (!g1.insert(e2) || 1 != e2.id) {
		console.log('Graph insert(e2) test failed');
	}

	if (1 != g1.find(ENTITY, 'User', 1).id) {
		onsole.log('Graph find(ENTITY, "User", 1) test failed');
	}

	if (1 != g1.find(ENTITY, 'Book', 1).id) {
		onsole.log('Graph find(ENTITY, "Book", 1) test failed');
	}

	if (!g1.remove(e1.nodeClass, e1.type, e1.id) || !g1.remove(e2)) {
		console.log('Graph remove() test failed');
	}

	setInterval(function() {
		let count = 1000000;
		let start = Date.now();
		for (let i = count; 0 < i; --i) {
			g1.insert(new Entity('User'));
			//g1.insert(0 == i % 2 ? new Entity('User') : new Entity('Book'));
			//g1.insert(0 == i % 2 ? new Action('Bought') : new Action('Read'));
		}
		console.log('Graph insert() tests passed', 'Operations', g1[0].User.size(), 'Time', Date.now() - start);
	}, 1000);

	//for (let i = g1.size() - 1; 0 <= i; --i) {
	//	for (let j = g1[i].size() - 1; 0 <= j; --j) {
	//		if (500000 != g1[i][j].size()) {
	//			console.log('Graph iterator[] test failed');
	//			break;
	//		}
	//	}
	//	if (i == 0) {
	//		console.log('Graph iterator[] test passed');
	//	}
	//}
	//
	//g1.clear();
	//if (0 != g1.size()) {
	//	console.log('Graph clear() test failed, Count', 0, 'Size', g1.size());
	//}

	console.log('Graph tests passed');
})();