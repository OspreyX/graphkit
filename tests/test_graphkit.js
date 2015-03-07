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
 *
 *
 * The following is a simple
 * recommendation engine that
 * tests GraphKit.
 *
 * In this recommendation, books will be purchased
 * and based on the books the User's friends bought
 * it will recommend a book for that user.
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

// Setup our initial input graph
let g1 = new Graph();

(function() {
	// add some books
	let start = Date.now();
	for (let i = 1000; 0 < i; --i) {
		let e1 = new Entity('Book');
		e1['title'] = 'Title ' + i;
		g1.insert(e1);
	}
	console.log('Books added (%d) Time %d', g1.Entity.Book.count(),  Date.now() - start);
})();

(function() {
	// add some users
	let start = Date.now();
	let males = 300;
	for (let i = 1000; 0 < i; --i) {
		let e1 = new Entity('User');
		e1['name'] = 'Name ' + i;
		g1.insert(e1);

		// let's make some female and some male
		e1.addGroup(--males ? 'male' : 'female');
	}
	console.log('Users added (%d) Time %d', g1.Entity.User.count(), Date.now() - start);
})();

(function() {
	// make users friends
	// we can do this by
	// giving each user a set of 10 friends randomly.
	let users = g1.Entity.User;
	let count = users.count();
	let start = Date.now();
	for (let i = count - 1; 0 <= i; --i) {
		let user = users[i];
		for (let j = 10; 0 < j; --j) {
			let index = Math.floor((Math.random() * count) + 1);
		}
	}
	console.log('Users added (%d) Time %d', g1.Entity.User.count(), Date.now() - start);
})();

let g2 = new Graph();
console.log(g2.Entity.Book.count(), g2.Entity.User.count());

g1.clear();