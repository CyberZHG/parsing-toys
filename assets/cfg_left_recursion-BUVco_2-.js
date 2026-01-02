import"./__vite-browser-external-CpYqa-Ba.js";import{s as o}from"./choose_examples-BjAtfSqC.js";import{C as s,g as c,u as l,s as m,a as p,b as i,c as u,d as n,e as b}from"./location_hash-_UlbSsyM.js";const d={example1:"Example 1: S -> S S + | S S * | a",example2:"Example 2: S -> ( L ) | a  L -> L , S | S",example3:"Example 3: A -> C | c d  B -> C | c d  C -> A | B | c d",example4:"Example 4: bexpr -> bexpr or bterm | bterm  bterm -> bterm and bfactor | bfactor  bfactor -> not bfactor | ( bexpr ) | true | false"},f={example1:`S -> S S + 
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
         | false`},a=document.querySelector("#button-cfg-left-recursion");a.addEventListener("click",()=>{const e=document.querySelector("#cfg-left-recursion-error-message"),t=new s,r=c();t.parse(r)?(t.leftRecursionElimination()?e.parentElement.hidden=!0:(e.textContent=t.errorMessage(),e.parentElement.hidden=!1),l(t.toString(),t.terminals(),t.nonTerminals())):(e.textContent=t.errorMessage(),e.parentElement.hidden=!1),m(r)});function x(e){n(f[e]),a.click()}o(d,x);p();i();u();n(b());a.click();
