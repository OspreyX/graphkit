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
g1.clear();

(function() {
	// add some books
	let start = Date.now();
	for (let i = process.argv[2] || 1; 0 < i; --i) {
		let user = new Entity('Book');
		user['title'] = 'Title ' + i;
		g1.insert(user);
		//g1.remove(user);
	}
	console.log('Books added (%d) Time %d', g1.Entity.Book.count,  Date.now() - start);
})();

(function() {
	// add some users
	let start = Date.now();
	for (let i = process.argv[2] || 1; 0 < i; --i) {
		let user = new Entity('User');
		user['name'] = 'Name ' + i;
		g1.insert(user);
	}
	console.log('Users added (%d) Time %d', g1.Entity.User.count, Date.now() - start);
})();

(function() {
	// make user friends,
	// we can do this by
	// giving each user a set of 10 friends randomly.
	let users = g1.Entity.User;
	let count = users.count;
	let start = Date.now();
	for (let i = count - 1; 0 <= i; --i) {
		let user = users[i];
		for (let j = 10; 0 < j; --j) {
			let index = Math.floor((Math.random() * count));
			let friend = new Bond('Friend');
			g1.insert(friend);
			friend.subject = user;
			friend.object = users[index];
		}
	}
	console.log('Users added (%d) Time %d', g1.Entity.User.count, Date.now() - start);
})();

(function() {
	// set the user to have read many books
	// in a single action call
	let users = g1.Entity.User;
	let books = g1.Entity.Book;
	let start = Date.now();
	for (let i = users.count - 1; 0 <= i; --i) {
		let user = users[i];
		for (let j = 10; 0 < j; --j) {
			let index = Math.floor((Math.random() * books.count));
			let read = new Action('Read');
			g1.insert(read);
			read.addSubject(user);
			read.addObject(books[index]);
			if (user.actions.find(read.nodeClass, read.type, read.id) != read) {
				console.log('Action addSubject(user) test failed.');
			}
			if (read.subjects.find(user.nodeClass, user.type, user.id) != user) {
				console.log('Action subjects test failed.');
			}
		}
	}
	console.log('Actions added (%d) Time %d', g1.Action.Read.count, Date.now() - start);
})();

(function() {
	// test groups
	let users = g1.Entity.User;
	let books = g1.Entity.Book;
	let reads = g1.Action.Read;
	let friends = g1.Bond.Friend;
	users[0].addGroup('test');
	books[0].addGroup('test');
	reads[0].addGroup('test');
	reads[1].addGroup('test');
	reads[1].removeGroup('test');
	users[0].removeGroup('test');
	friends[0].addGroup('test');
	g1.remove(books[0]);
	g1.remove(friends[0]);
	if (1 != g1.group('test').count) {
		console.log('Group test failed.', g1.group('test'));
	}
})();