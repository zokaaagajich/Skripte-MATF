# Statistika - Vezbe 11
# asistent: Tamara Milic 
# Zorana Gajic 

# LINEARNA REGRESIJA

# Zadatak - 2 
# protok saobracaja u hiljadama automobila u toku dana
x=c(8.3, 9.3, 12.1, 12.3, 17.0, 17.3, 24.3, 24.5, 33.6) 
# prisustva olova u kori drveca:
y=c(227, 312, 362, 521, 539, 728, 945, 1000, 1263)
# Oceniti koeficijente modela y=a*x+b

model = lm(y~x)
summary(model)

# Sta mozemo zakljuciti iz ovog modela? 
# Na osnovu R-squared=0.96 blizu 1 => mozemo zakljuciti da je dobar model
# Vidimo da je za intercept(odnosno clan b) p-vrednost 0.247 sto se smatra velikim
# (mislim da mozemo pamtiti da su velike sve bez zvezdica vrednosti)
# => prihvatamo H0 <=> b = 0 => b nije znacajno za ovaj model

# Da vizualizujemo
# iz rezultata modela
a = 41.318  
b = -73.356

# Kada bismo predstavili x i y, vidimo da lici da ima linearnu zavisnost
plot(x,y)
# regresiona prava
abline(b, a, col="red")

# Predvideti na osnovu modela za vrednosti x = 5, 13.3, 28, 30 i 100
predict(model, newdata = data.frame(x=c( 5, 13.3, 28, 30, 100)))

# Zadatak 3
# broj sati ucenja
x=c(4,9, 10, 14, 4, 7, 12, 22, 1, 3, 8, 11, 5, 6, 10, 11, 16, 13, 13, 10)
# rezultati testa.
y=c(390, 580, 650, 730, 410, 530, 600, 790, 350, 400, 590, 640,450, 520, 690, 690, 770,700, 730, 640)

model = lm(y~x)
summary(model)

# Sta mozemo zakljuciti iz ovog modela? 
# Na osnovu R-squared=0.87 tezi 1 => mozemo zakljuciti da je ok model, nije los.
# Ne znam bas koja je granica za ovo. Ako je ispod 0.7 mislim da zakljucujemo da je bzv
# Vidimo da je za intercept(odnosno clan b) p-vrednost 2.24e-11 *** sto se smatra malom vrednoscu
# => odbacujemo H0 <=> b != 0 => b je znacajno za ovaj model

# Vizualizacija
plot(x, y)
a = 25.326
b = 353.165
# regresiona prava
abline(b, a, col="red")

# Mozemo proveriti reziduale da li imaju lepe osobine, npr da ima normalnu raspodelu
reziduali = model$residuals
ks.test(reziduali, "pnorm")
# Vidimo da je p-vrednost jako mala sto znaci da bi nam bilo manje od nekog alfa 
# => da smo upali u kriticnu sekciju => odbaciti H0 => nije bas tako normalna raspodela 
# PROVERITI OVO 

# Zadatak 4
q=seq(-1, 1, 0.05)
w=q^2+rnorm(length(q),0, 0.05)

model = lm(w~q)
summary(model)

# Vidimo da za q nemamo zvezdica sto znaci da vrednost nije tako mala, odnosno velika je
# => prihvatamo H0 => q je nije znacajno za ovaj model
# I vidimo da je R-squared = 0.001 sto znaci da je los model

# Vizualizacija
plot(q, w)
a = model$coefficients[2]
b = model$coefficients[1]
abline(b, a, col="red")
# Uverili smo se da nam je model jako los, vidimo da ne prati uzorak uopste!
# Sto je i logicno jer ovaj uzorak lici na parabolu, a model nam je linearan po q 

# Zato mozemo probati kvadratni model
model2 = lm(w~I(q)+I(q^2))
summary(model2)
# Vidimo da je R-squared 0.9731 sto je jako dobar model
# Vidimo i sledece, za p vrednost I(q) velika vrednost => prihvatamo H0 =>
# q = 0 => nije znacajno, bas smo to i hteli, da zavisi vise od kvadrata
# A kod I(q^2) imamo jako malu vrednost => ZNACAJNA JE!

# Uverimo se vizualno
plot(w~q)
points(q, fitted(model2), col="red")
# Vidimo da bas lepo prati uzorak sto znaci da je jako dobar model

# Zadatak 4
