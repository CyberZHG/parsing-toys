import"./navbar-DwXUvKtM.js";import{s as o}from"./choose_examples-vr6Go3Mf.js";import{g as r,s as m,a as n}from"./cfg_editor-Bof7UeYh.js";import{u as l,s as c}from"./cfg_result-BlwN4V3V.js";import{C as p,s as i,a as E,g as u}from"./__vite-browser-external-BVNNUIbt.js";const x={example1:"Example 1: S -> A B  A -> a | ε  B -> b | ε",example2:"Example 2: S -> a S b | a b",example3:"Example 3: E -> E + T | T  T -> T * F | F  F -> ( E ) | id",example4:"Example 4: S -> A  A -> B  B -> a"},d={example1:`S -> A B
A -> a | ε
B -> b | ε`,example2:"S -> a S b | a b",example3:`E -> E + T | T
T -> T * F | F
F -> ( E ) | id`,example4:`S -> A
A -> B
B -> a`},t=document.querySelector("#button-cfg-cnf");t.addEventListener("click",()=>{const a=document.querySelector("#cfg-cnf-error-message"),e=new p,s=r();e.parse(s)?(e.toChomskyNormalForm(),l(e.toString(),e.terminals(),e.nonTerminals()),a.parentElement.hidden=!0):(a.textContent=e.errorMessage(),a.parentElement.hidden=!1),i(s)});function f(a){n(d[a]),t.click()}o(x,f);m();c();E();n(u());t.click();
