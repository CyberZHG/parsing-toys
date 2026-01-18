import"./__vite-browser-external-DW2qscRg.js";import{s as o}from"./choose_examples-BjAtfSqC.js";import{C as r,g as S,s as l,a as i,b as m,c as s,d as c}from"./location_hash-CQmiu6wv.js";import{u as p,s as E}from"./cfg_result-CcZunNg_.js";const u={example1:"Example 1: S -> i E t S | i E t S e S | a  E -> b",example2:"Example 2: S -> S S + | S S * | a",example3:"Example 3: S -> S + S | S S | ( S ) | S * | a",example4:"Example 4: A -> id | B | a  B -> C  C -> D  D -> id b"},d={example1:`S -> i E t S 
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
D -> id b`},a=document.querySelector("#button-cfg-left-factoring");a.addEventListener("click",()=>{const t=document.querySelector("#cfg-left-factoring-error-message"),e=new r,n=S();e.parse(n)?(e.leftFactoring(),p(e.toString(),e.terminals(),e.nonTerminals()),t.parentElement.hidden=!0):(t.textContent=e.errorMessage(),t.parentElement.hidden=!1),l(n)});function x(t){s(d[t]),a.click()}o(u,x);i();E();m();s(c());a.click();
