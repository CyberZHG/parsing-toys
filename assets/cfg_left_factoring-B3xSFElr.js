import"./__vite-browser-external-BgPlA5PU.js";import{s as o}from"./choose_examples-BjAtfSqC.js";import{g as r,s as S,a as s}from"./cfg_editor-Bof7UeYh.js";import{u as l,s as i}from"./cfg_result-CcZunNg_.js";import{C as m,s as c,a as p,g as E}from"./location_hash-DCeX6uqc.js";const u={example1:"Example 1: S -> i E t S | i E t S e S | a  E -> b",example2:"Example 2: S -> S S + | S S * | a",example3:"Example 3: S -> S + S | S S | ( S ) | S * | a",example4:"Example 4: A -> id | B | a  B -> C  C -> D  D -> id b"},d={example1:`S -> i E t S 
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
D -> id b`},a=document.querySelector("#button-cfg-left-factoring");a.addEventListener("click",()=>{const t=document.querySelector("#cfg-left-factoring-error-message"),e=new m,n=r();e.parse(n)?(e.leftFactoring(),l(e.toString(),e.terminals(),e.nonTerminals()),t.parentElement.hidden=!0):(t.textContent=e.errorMessage(),t.parentElement.hidden=!1),c(n)});function f(t){s(d[t]),a.click()}o(u,f);S();i();p();s(E());a.click();
