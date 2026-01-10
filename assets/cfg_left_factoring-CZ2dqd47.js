import"./__vite-browser-external-pUU1Sn4A.js";import{s as o}from"./choose_examples-BjAtfSqC.js";import{C as r,g as S,u as l,s as i,a as m,b as c,c as p,d as s,e as E}from"./location_hash-hONGlPoa.js";const u={example1:"Example 1: S -> i E t S | i E t S e S | a  E -> b",example2:"Example 2: S -> S S + | S S * | a",example3:"Example 3: S -> S + S | S S | ( S ) | S * | a",example4:"Example 4: A -> id | B | a  B -> C  C -> D  D -> id b"},d={example1:`S -> i E t S 
   | i E t S e S 
   | a
E -> b`,example2:`S -> S S + 
   | S S * 
   | a`,example3:`S -> S + S 
   | S S 
   | ( S ) 
   | S * 
   | a`,example4:`A -> id | B | a
B -> C
C -> D
D -> id b`},a=document.querySelector("#button-cfg-left-factoring");a.addEventListener("click",()=>{const t=document.querySelector("#cfg-left-factoring-error-message"),e=new r,n=S();e.parse(n)?(e.leftFactoring(),l(e.toString(),e.terminals(),e.nonTerminals()),t.parentElement.hidden=!0):(t.textContent=e.errorMessage(),t.parentElement.hidden=!1),i(n)});function x(t){s(d[t]),a.click()}o(u,x);m();c();p();s(E());a.click();
