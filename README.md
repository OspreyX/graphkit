
#GraphKit  

A highly performant native graph data store for Node.js. Welcome to GraphKit. Written completely in C/C\+\+, GraphKit is woven into the fabric of Node.js. The power of graph data models may now be leveraged within the Node.js environment and can easily scale to complex networked systems. Classic mathematics is at the core of GraphKit, offering developers synthesized tools for machine learning and predictive analytics that expand the imagination of technology and science.

##Installation

npm install graphkit

##Documentation

Coming soon!

##A Taste

```javascript

let gk = require('graphkit');
let Graph = gk.Graph;

// Create a Graph instance.
let g = new Graph();

// Create a User Entity.
// Entities represent a person, place, or thing -- a noun. 
let u1 = g.createEntity('User');

// Give the user some properties.
u1['name'] = 'Eve';
u1['age'] = 26;

// Add Eve to a group. This creates a subset in the Graph named "female".
u1.addGroup('female');

// Create a Read Action.
// Actions represent events, and may capture the subject and objects 
// involved in that action.
let a1 = g.createAction('Read');

// Record the session the Action occurred in. 
 a1['session'] = 123;
 
// Set Eve as the subject. 
a1.subjects().insert(u1);
 
// Create some Book Entities. 
let b1 = g.createEntity('Book');
b1['title'] = 'Deep C Secrets';
b1.addGroup('Thriller');
 
let b2 = g.createEntity('Book');
b2['title'] = 'Mastering Node.js';
b2.addGroup('Suspense');
b2.addGroup('Favourite');
 
// Add the books as the objects for the Action. 
a1.objects().insert(b1);
a1.objects().insert(b2);

```

##License

[AGPL v3.0](http://choosealicense.com/licenses/agpl-3.0/)

##Contributors

[Daniel Dahan](https://github.com/danieldahan)