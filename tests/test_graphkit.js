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

let gk = require('../');
const NODE = gk.NODE;
const ENTITY = gk.ENTITY;
const ACTION = gk.ACTION;
const BOND = gk.BOND;
let Entity = gk.Entity;
let Action = gk.Action;
let Bond = gk.Bond;
let Index = gk.Index;
let Cluster = gk.Cluster;
let Graph = gk.Graph;
let GraphSet = gk.GraphSet;
let GraphMultiset = gk.GraphMultiset;
let Set = gk.Set;
let Multiset = gk.Multiset;

(function() {
	console.log('\nEntity Tests\n');

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

	if ('Entity' != e.nodeClassToString()) {
		console.log('Entity nodeClassToString() test failed');
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

	if (!e.removeGroup('admin')) {
		console.log('Entity removeGroup("admin") test failed');
	}

	if (!e.removeGroup('female')) {
		console.log('Entity removeGroup("female") test failed');
	}

	if (0 != e.groupSize()) {
		console.log('Entity groupSize() test failed');
	}

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

	console.log('Entity tests executed');
})();

(function() {
	console.log('\nAction Tests\n');

	let a;
	try {
		a = new Action();
		console.log('Action empty initialization test failed');
	} catch (error) {}

	try {
		a = new Action(1);
		console.log('Action numeric initialization test failed');
	} catch (error) {}

	a = new Action('Clicked');

	if (!(a instanceof Action)) {
		console.log('Action instanceof test failed');
	}

	if (ACTION != a.nodeClass) {
		console.log('Action nodeClass test failed');
	}

	if ('Action' != a.nodeClassToString()) {
		console.log('Action nodeClassToString() test failed');
	}

	if ('Clicked' != a.type) {
		console.log('Action type test failed');
	}

	if (0 != a.id) {
		console.log('Action id test failed');
	}

	if (!a.addGroup('ios')) {
		console.log('Action addGroup("ios") test failed');
	}

	if (!a.addGroup('mobile')) {
		console.log('Action addGroup("mobile") test failed');
	}

	if (2 != a.groupSize()) {
		console.log('Action groupSize() test failed');
	}

	if (!a.hasGroup('ios')) {
		console.log('Action hasGroup("ios") test failed');
	}

	if (!a.hasGroup('mobile')) {
		console.log('Action hasGroup("mobile") test failed');
	}

	if (!a.removeGroup('ios')) {
		console.log('Action removeGroup("ios") test failed');
	}

	if (!a.removeGroup('mobile')) {
		console.log('Action removeGroup("mobile") test failed');
	}

	if (0 != a.groupSize()) {
		console.log('Action groupSize() test failed');
	}

	a['campaign'] = 'holidays';
	a['session'] = 123;

	if ('holidays' != a['campaign']) {
		console.log('Action insert a["campaign"] test failed');
	}

	if (123 != a['session']) {
		console.log('Action insert a["session"] test failed');
	}

	delete a['campaign'];
	delete a['session'];

	if ('holidays' == a['campaign']) {
		console.log('Action remove a["campaign"] test failed');
	}

	if (123 == a['session']) {
		console.log('Action remove a["session"] test failed');
	}

	let g1 = new Graph();
	let a1 = g1.createAction('Read');
	let e1 = g1.createEntity('User');
	a1.subjects.insert(e1);
	a1.subjects.insert(e1);
	a1.objects.insert(g1.createEntity('Book'));
	a1.objects.insert(g1.createEntity('Book'));

	if (1 != a1.subjects.size() || 2 != a1.objects.size()) {
		console.log('Action relationship test failed');
	}

	console.log('Action tests executed');
})();

(function() {
	console.log('\nBond Tests\n');

	let b;
	try {
		b = new Bond();
		console.log('Bond empty initialization test failed');
	} catch (error) {}

	try {
		b = new Bond(1);
		console.log('Bond numeric initialization test failed');
	} catch (error) {}

	b = new Bond('Relationship');

	if (!(b instanceof Bond)) {
		console.log('Bond instanceof test failed');
	}

	if (BOND != b.nodeClass) {
		console.log('Bond nodeClass test failed');
	}

	if ('Bond' != b.nodeClassToString()) {
		console.log('Bond nodeClassToString() test failed');
	}

	if ('Relationship' != b.type) {
		console.log('Bond type test failed');
	}

	if (0 != b.id) {
		console.log('Bond id test failed');
	}

	if (!b.addGroup('family')) {
		console.log('Bond addGroup("family") test failed');
	}

	if (!b.addGroup('cousin')) {
		console.log('Bond addGroup("cousin") test failed');
	}

	if (2 != b.groupSize()) {
		console.log('Bond groupSize() test failed');
	}

	if (!b.hasGroup('family')) {
		console.log('Bond hasGroup("family") test failed');
	}

	if (!b.hasGroup('cousin')) {
		console.log('Bond hasGroup("cousin") test failed');
	}

	if (!b.removeGroup('family')) {
		console.log('Bond removeGroup("family") test failed');
	}

	if (!b.removeGroup('cousin')) {
		console.log('Bond removeGroup("cousin") test failed');
	}

	if (0 != b.groupSize()) {
		console.log('Bond groupSize() test failed');
	}

	b['confirmed'] = true;
	b['session'] = 123;

	if (true != b['confirmed']) {
		console.log('Bond insert b["confirmed"] test failed');
	}

	if (123 != b['session']) {
		console.log('Bond insert b["session"] test failed');
	}

	delete b['confirmed'];
	delete b['session'];

	if (true == b['confirmed']) {
		console.log('Bond remove b["confirmed"] test failed');
	}

	if (123 == b['session']) {
		console.log('Bond remove b["session"] test failed');
	}

	let g1 = new Graph();
	let e1 = g1.createEntity('User');
	let e2 = g1.createEntity('Book');
	b.subject = e1;
	b.object = e2;

	if ('User' != b.subject.type || 'Book' != b.object.type) {
		console.log('Bond relationship test failed');
	}

	console.log('Bond tests executed');
})();

(function() {
	console.log('\nIndex Tests\n');

	let i1 = new Index(ENTITY, 'User');
	let e1 = new Entity('User');
	let e2 = new Entity('Book');
	let a1 = new Action('Read');

	if (ENTITY != i1.nodeClass) {
		console.log('Index nodeClass test failed - got', i1.nodeClassToString());
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
	console.log('Index insert() stress tests passed', 'Operation Count', count, 'Time', Date.now() - start);

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

	console.log('Index tests executed');
})();

(function() {
	console.log('\nCluster Tests\n');

	let c1 = new Cluster(ENTITY);
	let e1 = new Entity('User');
	let e2 = new Entity('Book');

	if (ENTITY != c1.nodeClass) {
		console.log('Cluster nodeClass test failed - got', c1.nodeClassToString());
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
	console.log('Cluster insert() stress tests passed', 'Operation Count', count, 'Time', Date.now() - start);

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

	console.log('Cluster tests executed');
})();

(function() {
	console.log('\nGraph Tests\n');

	let g1 = new Graph();
	let e1 = new Entity('User');
	let e2 = new Entity('Book');

	if (0 != g1.size()) {
		console.log('Graph size() test failed, Count', 0, 'Size', g1.size());
	}

	e1.addGroup('admin');
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

	//if (e1.addGroup('admin') || e1.id != g1.Group['admin'].find(e1).id) {
	//	console.log('Graph Group["admin"] test failed', g1.Group['admin'].find(e1));
	//}

	if (!g1.remove(e1.nodeClass, e1.type, e1.id) || !g1.remove(e2)) {
		console.log('Graph remove() test failed');
	}

	let count = 1000000;
	let start = Date.now();
	for (let i = count; 0 < i; --i) {
		g1.insert(0 == i % 2 ? new Entity('User') : new Entity('Book'));
		g1.insert(0 == i % 2 ? new Action('Bought') : new Action('Read'));
	}
	console.log('Graph insert() stress tests passed', 'Operation Count', g1.Entity.User.size() + g1.Entity.Book.size() + g1.Action.Bought.size() + g1.Action.Read.size(), 'Time', Date.now() - start);

	for (let i = g1.size() - 1; 0 <= i; --i) {
		for (let j = g1[i].size() - 1; 0 <= j; --j) {
			if (500000 != g1[i][j].size()) {
				console.log('Graph iterator[] test failed');
				break;
			}
		}
		if (i == 0) {
			console.log('Graph iterator[] test passed');
		}
	}

	g1.clear();
	if (0 != g1.size()) {
		console.log('Graph clear() test failed, Count', 0, 'Size', g1.size());
	}

	console.log('Graph tests executed');
})();

(function() {
	console.log('\nGraphSet Tests\n');

	let g1 = new Graph();
	let s1 = new GraphSet(g1);
	let s2 = g1.Set();
	let e1 = new Entity('User');
	let e2 = new Entity('User');

	if (!s1.insert(e1) || s1.insert(e1) || 1 != s1.size() || g1.Entity.User[0] != e1) {
		console.log('GraphSet insert(e1) test failed');
	}

	if (e1 != s1.find(ENTITY, 'User', 1)) {
		console.log('GraphSet find(ENTITY, "User", 1) test failed', s1.find(ENTITY, 'User', 1));
	}

	if (!s1.insert(e2) || 2 != s1.size() || !s1.remove(e2) || 1 != s1.size() || !s1.remove(e1.nodeClass, e1.type, e1.id) || 0 != s1.size()) {
		console.log('GraphSet remove() test failed');
	}

	let count = 1000000;
	let start = Date.now();
	for (let i = count; 0 < i; --i) {
		s2.insert(new Entity('Book'));
	}
	if (g1.Entity.Book.size() != s2.size()) {
		console.log('GraphSet insert stress test failed');
	} else {
		console.log('GraphSet insert() stress tests passed', 'Operation Count', g1.Entity.Book.size(), 'Time', Date.now() - start);
	}

	s1.clear();
	s2.clear();
	if (0 != s1.size() || 0 != s2.size()) {
		console.log('GraphSet clear() test failed, Count', 0, 'Size', s1.size(), s2.size());
	}

	console.log('GraphSet tests executed');
})();

(function() {
	console.log('\nGraphMultiset Tests\n');

	let g1 = new Graph();
	let s1 = new GraphMultiset(g1);
	let s2 = g1.Multiset();
	let e1 = new Entity('User');
	let e2 = new Entity('User');

	if (!s1.insert(e1) || !s1.insert(e1) || 2 != s1.size() || g1.Entity.User[0] != e1) {
		console.log('GraphMultiset insert(e1) test failed');
	}

	if (e1 != s1.find(ENTITY, 'User', 1)) {
		console.log('GraphMultiset find(ENTITY, "User", 1) test failed', s1.find(ENTITY, 'User', 1));
	}

	if (!s1.insert(e2) || 3 != s1.size() || !s1.remove(e2) || 2 != s1.size() || !s1.remove(e1.nodeClass, e1.type, e1.id) || 0 != s1.size()) {
		console.log('GraphMultiset remove() test failed');
	}

	let count = 1000000;
	let start = Date.now();
	for (let i = count; 0 < i; --i) {
		s2.insert(new Entity('Book'));
	}
	if (g1.Entity.Book.size() != s2.size()) {
		console.log('GraphMultiset insert stress test failed');
	} else {
		console.log('GraphMultiset insert() stress tests passed', 'Operation Count', g1.Entity.Book.size(), 'Time', Date.now() - start);
	}

	s1.clear();
	s2.clear();
	if (0 != s1.size() || 0 != s2.size()) {
		console.log('GraphMultiset clear() test failed, Count', 0, 'Size', s1.size(), s2.size());
	}

	console.log('GraphMultiset tests executed');
})();

(function() {
	console.log('\nSet Tests\n');

	let g1 = new Graph();
	let s1 = new Set();
	let e1 = g1.createEntity('User');
	let e2 = g1.createEntity('User');

	if (!s1.insert(e1) || s1.insert(e1) || 1 != s1.size() || g1.Entity.User[0] != e1) {
		console.log('Set insert(e1) test failed');
	}

	if (e1 != s1.find(ENTITY, 'User', 1)) {
		console.log('Set find(ENTITY, "User", 1) test failed', s1.find(ENTITY, 'User', 1));
	}

	if (!s1.insert(e2) || 2 != s1.size() || !s1.remove(e2) || 1 != s1.size() || !s1.remove(e1.nodeClass, e1.type, e1.id) || 0 != s1.size()) {
		console.log('Set remove() test failed');
	}

	s1.clear();
	let count = 1000000;
	let start = Date.now();
	for (let i = count; 0 < i; --i) {
		let e = new Entity('Book');
		if (0 == i % 2) {
			g1.insert(e);
			s1.insert(e);
			s1.insert(e);
		} else {
			try {
				s1.insert(e);
			} catch (error) {}
		}
	}
	if (count / 2 != s1.size()) {
		console.log('Set insert stress test failed', s1.size());
	} else {
		console.log('Set insert() stress tests passed', 'Operation Count', s1.size(), 'Time', Date.now() - start);
	}

	s1.clear();
	if (0 != s1.size()) {
		console.log('Set clear() test failed, Count', 0, 'Size', s1.size());
	}

	console.log('Set tests executed');
})();

(function() {
	console.log('\nMultiset Tests\n');

	let g1 = new Graph();
	let s1 = new Multiset();
	let e1 = g1.createEntity('User');
	let e2 = g1.createEntity('User');

	if (!s1.insert(e1) || !s1.insert(e1) || 2 != s1.size() || g1.Entity.User[0] != e1) {
		console.log('Multiset insert(e1) test failed', s1.size());
	}

	if (e1 != s1.find(ENTITY, 'User', 1)) {
		console.log('Multiset find(ENTITY, "User", 1) test failed', s1.find(ENTITY, 'User', 1));
	}

	if (!s1.insert(e2) || 3 != s1.size() || !s1.remove(e2) || 2 != s1.size() || !s1.remove(e1.nodeClass, e1.type, e1.id) || 0 != s1.size()) {
		console.log('Multiset remove() test failed');
	}

	s1.clear();
	let count = 1000000;
	let start = Date.now();
	for (let i = count; 0 < i; --i) {
		let e = new Entity('Book');
		if (0 == i % 2) {
			g1.insert(e);
			s1.insert(e);
			s1.insert(e);
		} else {
			try {
				s1.insert(e);
			} catch (error) {}
		}
	}
	if (count != s1.size()) {
		console.log('Multiset insert stress test failed', s1.size());
	} else {
		console.log('Multiset insert() stress tests passed', 'Operation Count', s1.size(), 'Time', Date.now() - start);
	}

	s1.clear();
	if (0 != s1.size()) {
		console.log('Multiset clear() test failed, Count', 0, 'Size', s1.size());
	}

	console.log('Multiset tests executed');
})();