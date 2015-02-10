
#GraphKit  

A highly performant native graph data store for Node.js. Welcome to GraphKit. Written completely in C/C\+\+, GraphKit is woven into the fabric of Node.js. The power of graph data models may now be leveraged within the Node.js environment and can easily scale to complex networked systems. Classic mathematics is at the core of GraphKit, offering developers synthesized tools for machine learning and predictive analytics that expand the imagination of technology and science.

##Installation

npm install graphkit

##Documentation

[graphkit.io](http://graphkit.io)

##A Taste

```javascript
// command: node --harmony filename

'use strict';
'use esnext';

// Require package Classes.
let gk = require('graphkit');
let Graph = gk.Graph;
let Entity = gk.Entity;

// Create a Graph instance.
let g1 = new Graph();

// Create a User Entity.
let u1 = new Entity('User');

// Give u1 some properties.
u1['name'] = 'Eve';
u1['age'] = 26;

// Add u1 to a group. This creates a subset in the Graph named 'female'.
u1.addGroup('Female');

// Output: 0, before inserting in the Graph.
console.log(u1.id);

// Output: 1, after inserting in the Graph.
g1.insert(u1);
console.log(u1.id);

// Create a Book Entity using Graph factory methods.
let b1 = g1.createEntity('Book');
b1['title'] = 'Deep C Secrets';
b1.addGroup('Thriller');

// Output: 1, b1 was automatically inserted in the Graph.
console.log(b1.id);

// Create a Read Action using Graph factory methods.
let a1 = g1.createAction('Read');

// Output: 1, a1 was automatically inserted in the Graph.
console.log(a1.id);

// Record the session the Action occurred in.
a1['session'] = 123;

// Insert u1 as a subject for the Action.
a1.subjects().insert(u1);

// Create another Book Entity.
let b2 = g1.createEntity('Book');
b2['title'] = 'Mastering Node.js';
b2.addGroup('Suspense');
b2.addGroup('Favourite');

// Output: 2, b2 was automatically inserted in the Graph.
console.log(b2.id);

// Add the books as the objects for the Action.
a1.objects().insert(b1);
a1.objects().insert(b2);

// Output: 1
console.log(a1.subjects().size());

// Output: 2
console.log(a1.objects().size());

// Output: 'Eve read 2 books.'
console.log(a1.subjects()[0]['name'], a1.type.toLowerCase(), 
a1.objects().size(), 
a1.objects()[0].type.toLowerCase() + 's.');

// Output: 'There is 1 user and 2 books in the graph.'
console.log('There is', g1.Entity.User.size(), 
g1.Entity.User.type.toLowerCase(), 'and', 
g1.Entity.Book.size(), 
g1.Entity.Book.type.toLowerCase() + 's', 
'in the graph.');
```

##License

[AGPLv3](http://choosealicense.com/licenses/agpl-3.0/)

##Contributors

[Daniel Dahan](https://github.com/danieldahan)