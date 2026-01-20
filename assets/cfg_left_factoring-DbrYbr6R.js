import"./navbar-DcTHfFKV.js";import{s as r}from"./choose_examples-vr6Go3Mf.js";import{g as c,s as S,a as o}from"./cfg_editor-Bof7UeYh.js";import{u as l,s as i}from"./cfg_result-BlwN4V3V.js";import{C as m,s as p,a as d,g as u}from"./__vite-browser-external-XlBbu2m0.js";const E={example1:"Example 1: S -> i E t S | i E t S e S | a  E -> b",example2:"Example 2: S -> S S + | S S * | a",example3:"Example 3: S -> S + S | S S | ( S ) | S * | a",example4:"Example 4: A -> id | B | a  B -> C  C -> D  D -> id b"},x={example1:`S -> i E t S 
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
D -> id b`},a=document.querySelector("#button-cfg-left-factoring"),s=document.querySelector("#checkbox-expand");a.addEventListener("click",()=>{const t=document.querySelector("#cfg-left-factoring-error-message"),e=new m,n=c();e.parse(n)?(e.leftFactoring(s.checked),l(e.toString(),e.terminals(),e.nonTerminals()),t.parentElement.hidden=!0):(t.textContent=e.errorMessage(),t.parentElement.hidden=!1),p(n)});s.addEventListener("change",()=>{a.click()});function g(t){o(x[t]),a.click()}r(E,g);S();i();d();o(u());a.click();
