
#GraphKit  

A highly performant native graph data store for Node.js. Welcome to GraphKit. Written completely in C/C\+\+, GraphKit is woven into the fabric of Node.js. The power of graph data models may now be leveraged within the Node.js environment and can easily scale to complex networked systems. Classic mathematics is at the core of GraphKit, offering developers synthesized tools for machine learning and predictive analytics that expand the imagination of technology and science.

##Installation

npm install graphkit

##Documentation

[graphkit.io](http://graphkit.io)

##A Taste

```javascript
// Require package Classes.
let gk = require('graphkit');
let Graph = gk.Graph;

// Create a Graph instance.
let g1 = new Graph();

// create a User Entity
let u1 = g1.createEntity('User');

// Give u1 some properties.
u1['name'] = 'Eve';
u1['age'] = 26;

// Add u1 to a group. This creates a subset in the Graph named "female".
u1.addGroup('female');

// Create a Read Action using Graph factory methods.
let a1 = g1.createAction('Read');

// Output: 1, a1 was automatically indexed in the Graph.
console.log(a1.id);

// Record the session the Action occurred in.
a1['session'] = 123;

// Insert u1 as a subject for the Action.
a1.subjects().insert(u1);

// Create some Book Entity Nodes.
let b1 = g1.createEntity('Book');
b1['title'] = 'Deep C Secrets';
b1.addGroup('Thriller');

let b2 = g1.createEntity('Book');
b2['title'] = 'Mastering Node.js';
b2.addGroup('Suspense');
b2.addGroup('Favourite');

// Add the books as the objects for the Action.
a1.objects().insert(b1);
a1.objects().insert(b2);

// Output: 1
console.log(a1.subjects().size());

// Output: 2
console.log(a1.objects().size());
```

##License

[AGPLv3](http://choosealicense.com/licenses/agpl-3.0/)

##Contributors

[Daniel Dahan](https://github.com/danieldahan)