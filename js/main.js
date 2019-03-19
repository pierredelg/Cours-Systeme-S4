//const nom = `Regina`;
//let html = `images/` + nom.toLowerCase() + `.jpg`;
//document.querySelector(".pizzasContainer").innerHTML = html;

//let html;

let noms = ['Regina', 'Napolitaine', 'Spicy'];

  noms = [
    {
        nom: 'Regina',
        base: 'tomate',
        ingredients: [
            {id:1, nom: 'jambon'},
            {id:1, nom: 'champignons'},
            {id:1, nom: 'fromage'},
        ]
    },
    {
        nom: 'Napolitaine',
        base: 'tomate',
        ingredients: [
            {id:1, nom: 'jambon'},
            {id:1, nom: 'anchois'},
            {id:1, nom: 'fromage'},
        ]
    },
    {
        nom: 'Spicy',
        base: 'tomate',
        ingredients: [
            {id:1, nom: 'jambon'},
            {id:1, nom: 'piment'},
            {id:1, nom: 'fromage'},
        ]
    }
];


/*
METHODE FOR


let html = "";

for (let i = 0; i < noms.length;i++ ){
    
    let html2 = `images/` + noms[i].nom.toLowerCase() + `.jpg`;
    
    html += `<article class="media">
        <a href="${html2}" >
            <img src="${html2}" width="246" height="138" />
            <h4>${noms[i].nom}</h4>
                <ul>`;
                for(let j = 0;j < noms[0].ingredients.length;j++){
                    html += `<li>${noms[i].ingredients[j].nom}</li>`;
                }
                html +=`</ul>
        </a>
    </article>`;
}
*/


/*
METHODE FOR EACH
*/
let html = "";
noms.forEach(function(pizza) {

    let html2 = `images/` + pizza.nom.toLowerCase() + `.jpg`;
    
    html += `<article class="media">
        <a href="${html2}">
            <img src="${html2}" width="246" height="138" />
            <h4>${pizza.nom}</h4>
                <ul>`;
                pizza.ingredients.forEach(function(ingred) {
                     html += `<li>${ingred.nom}</li>`;
                });
                 html += `</ul>
        </a>
    </article>`;
});

document.querySelector(".pizzasContainer").innerHTML = html;