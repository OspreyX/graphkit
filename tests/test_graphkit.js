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

let g1 = new Graph();
//console.log(g1.Entity.User.size());
console.log('Clear');
g1.clear();

console.log('iterator');
let start = Date.now();
for (let i = process.argv[2] || 1; 0 < i; --i) {

	let e1 = new Entity('User');
	//console.log(e1);
	g1.insert(e1);
	//console.log(e1);

	//let u1 = g1.find(gk.ENTITY, 'User', i) || g1.createEntity('User');
	//u1['name'] = 'Eve';

	//let u2 = g1.find(gk.ENTITY, 'User', i) || g1.createEntity('User');
	//u2['name'] = 'Daniel';
	//
	//let u3 = g1.find(gk.ENTITY, 'User', i) || g1.createEntity('User');
	//u3['name'] = 'Adam';
	//
	//let b1 = g1.find(gk.BOND, 'Friend', i) || g1.createBond('Friend');
	//b1.subject = u1;
	//b1.object = u2;
	//
	//let b2 = g1.find(gk.BOND, 'Friend', i) || g1.createBond('Friend');
	//b2.subject = u3;
	//b2.object = u2;
	//
	//let m1 = g1.find(gk.ENTITY, 'Message', i) || g1.createEntity('Message');
	//m1['text'] = 'Hey, how are you?';
	//
	//let a1 = g1.find(gk.ACTION, 'Emailed', i) || g1.createAction('Emailed');
	//a1.addSubject(u1);
	//a1.addSubject(u2);
	//a1.addObject(m1);
}
console.log('Basic Time %d', Date.now() - start);
