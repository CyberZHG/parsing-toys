import"./__vite-browser-external-DqTXNKOh.js";import{s as o}from"./choose_examples-BjAtfSqC.js";import{C as s,g as l,s as c,a as m,b as p,c as n,d as i}from"./location_hash-C115mlI6.js";import{u,s as x}from"./cfg_result-DvkoiS-f.js";const f={example1:"Example 1: S -> S S + | S S * | a",example2:"Example 2: S -> ( L ) | a  L -> L , S | S",example3:"Example 3: A -> C | c d  B -> C | c d  C -> A | B | c d",example4:"Example 4: bexpr -> bexpr or bterm | bterm  bterm -> bterm and bfactor | bfactor  bfactor -> not bfactor | ( bexpr ) | true | false",example5:"Example 5: A -> B  B -> C  C -> D  D -> A"},b={example1:`S -> S S + 
   | S S * 
   | a`,example2:`S -> ( L ) 
   | a L -> L , S 
   | S`,example3:`A -> C | c d
B -> C | c d
C -> A | B | c d`,example4:`  bexpr -> bexpr or bterm
         | bterm
  bterm -> bterm and bfactor
         | bfactor
bfactor -> not bfactor
         | ( bexpr )
         | true
         | false`,example5:`A -> B
B -> C
C -> D
D -> A`},a=document.querySelector("#button-cfg-left-recursion");a.addEventListener("click",()=>{const e=document.querySelector("#cfg-left-recursion-error-message"),t=new s,r=l();t.parse(r)?(t.leftRecursionElimination()?e.parentElement.hidden=!0:(e.textContent=t.errorMessage(),e.parentElement.hidden=!1),u(t.toString(),t.terminals(),t.nonTerminals())):(e.textContent=t.errorMessage(),e.parentElement.hidden=!1),c(r)});function d(e){n(b[e]),a.click()}o(f,d);m();x();p();n(i());a.click();
