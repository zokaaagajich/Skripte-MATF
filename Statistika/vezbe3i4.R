# Statistika - Vezbe 3 i 4
# asistent: Tamara Milic 
# Zorana Gajic 

# Zadatak 2 
# a) Ocena za p ako je X~B(N,p) gde je p nepoznato. 
simulacija_a <- function(n, N, p) {
  uzorak = rbinom(n, N, p)
  p.hat = mean(uzorak)/N
  return(p.hat)
}
simulacija_a(100, 10, 0.5)

# b) Ocena za theta za X~U[0, theta]
simulacija_b <- function(n, theta) {
  uzorak = runif(n, 0, theta)
  thetha.hat = 2*mean(uzorak)
  return(thetha.hat)
}
simulacija_b(1000, 1)

# c) Ocena za p gde je    X : ( -1    0    1  )
#                               p/2  p/2  1-p

# Prvo treba da generisemo ovakvu raspodelu jer je nemamo
# ugradjenu!

generisi_uzorak <- function(p) {
  u <- runif(1)
  if (u <= p/2)
    return (-1)
  else if (u > p/2 & u<= p) 
    return(0)
  else return(1)
}

simulacija_c <- function(n, p) {
  uzorak <- replicate(n, generisi_uzorak(p))
  p.hat <- 2/3*(1-mean(uzorak))
  return(p.hat)
}

simulacija_c(1000, 0.5)

# d) alfa i beta za X~gama(alfa, beta)

simulacija_d <- function(n, alfa, beta) {
  uzorak <- rgamma(n, shape = alfa, rate = beta)
  alfa.hat = mean(uzorak)^2/var(uzorak)
  beta.hat = mean(uzorak)/var(uzorak)
  return(c(alfa.hat, beta.hat))
}
simulacija_d(1000, 1, 2)  

# e) a i b iz X~U[a,b]
simulacija_e <- function(n,a,b) {
  uzorak <- runif(n,a,b)
  a.hat = mean(uzorak)-sqrt(3*var(uzorak))
  b.hat = mean(uzorak)+sqrt(3*var(uzorak))
  return(c(a.hat, b.hat))
}
simulacija_e(1000, 1, 5)  

# Domaci: 
# 1. Ocena za lambda ako je X~P(lambda) 
simulacija1 <- function(n, l) {
  uzorak <- rpois(n, lambda = l) 
  lambda.hat = mean(uzorak)
  return(lambda.hat)
}
simulacija1(1000, 2)

# 2. Ocena za m, thetha^2 ako je X~N(m, thetha^2)
simulacija2 <- function(n, m, thetha2) {
  uzorak <- rnorm(n, m, thetha2)
  m.hat = mean(uzorak)
  thetha2.hat = sd(uzorak) # ili var(uzorak)?
  return(c(m.hat, thetha2.hat))
}
simulacija2(100, 1, 2)
  
# Zadatak 1
# Generisemo uzorak iz U[theta, 0]
uzorak <- runif(n, -2, 0)
theta1.hat = (n+1)/n*min(uzorak)
theta2.hat = 2*mean(uzorak)

# Hocemo da ocenimo disperzije
ocena_disperzija <- function(n, theta) {
  uzorak <- runif(n, theta, 0)
  theta1.hat = ((n+1)/n)*min(uzorak)
  theta2.hat = 2*mean(uzorak)
  return(c(theta1.hat, theta2.hat))
}
ocena_disperzija(100, -5)

r<- replicate(1000, ocena_disperzija(100,-5))
rowMeans(r)
ocenadisp1 = var(r[1, ])
ocenadisp2 = var(r[2, ])

# Vidimo da je prva disperzija manja => bolja!
# Da vidimo koliko su ove dve disperzije blizu tacnih disperzija
theta_disp = -5
n = 100
disp1 = (-theta_disp)^2/(n*(n+2))
disp2 = theta_disp^2/(3*n)

# Vidimo da su disp1 i ocenadisp1 i tako za disp 2 jako slicne sto znaci da smo dobro ocenili disperzije


# Zadatak 3
simulacija_3 <- function(n, l) {
  uzorak <- rexp(n, l)
  lambda.hat = 1/mean(uzorak)
  return(lambda.hat)
}
simulacija_3(100, 1)

# Ocekivanje ocene lambda.hat
r <- replicate(1000, simulacija_3(100, 2))
mean(r)
# Sta smo mi ovde zapravo uradili. Ocekivanje necega nam je kao srednja vrednost, znaci, mi smo
# 1000 puta ocenili lambda i dobijali neko lambda.hat i od 1000 lambda.hat-ova smo uzeli srednju vrednost
# i dobili da je jako blizu dva, sto je dobra ocena


# Zadatak 6 - MMV
# a) Oceni lambda za X~P(lambda)
simulacija_6a <- function(n, l) {
  uzorak <- rpois(n, l)
  lambda.hat = sum(uzorak)/n
  return(lambda.hat)
}
simulacija_6a(1000, 3)

# Ili koristiti ugradjenu fju za ocenjivanje metodom maksimalne verodostojnosti fitdistr
install.packages("MASS")
library(MASS)
uzorak <- rpois(1000, 3)
fitdistr(uzorak, "Poisson")

# b) X~Ber(p)
generisi_uzorak_bernuli <- function(p) {
  x <- runif(1)
  if (x <= p)
    return(1)
  else 
    return(0)
}
simulacija_6b <- function(n, p) {
  uzorak <- replicate(100, generisi_uzorak_bernuli(p))
  p.hat = mean(uzorak)
  return(p.hat)
}
simulacija_6b(1000, 0.4)

# c) p za X~G(p)
n = 1000
p = 0.8
# I nacin
uzorak <- rgeom(n, prob=p)
p.hat = n/sum(uzorak + 1) # + 1 zbog razlike u def rgeom u R-u
p.hat

fitdistr(uzorak, "geometric")

# d) Ocena za p, gde je X:( -1   0   1   )
#                            p   p  1-2p
generisi_uzorak_6d <- function(p) {
  u <- runif(1)
  if(u<=p)
    return(-1)
  else if(u>p & u<2*p)
    return(0)
  else return(1)
}
simulacija_6d <- function(n, p) {
  uzorak = replicate(1000, generisi_uzorak_6d(0.5))
  p.hat = (n-sum(uzorak==1))/2*n
  return(p.hat)
}
simulacija_6d(1000, 0.2)

# Zadatak 7 
# Oceni P{X>=3} za X~B(5, p) 
n <- 1000
N = 5
p = 0.5
uzorak <- rbinom(n, N, p)
p.hat <-mean(uzorak)/5
p.hat
 
# P{x>=3} = 1-P{x<3} = 1-P{x<=2}
1 - pbinom(2, N, p)
# a ocena
1 - pbinom(2, N, p.hat)


# Zadatak 8
# a) X~E(lambda) 
simulacija_8a <- function(n, l) {
  uzorak = rexp(n, l)
  lambda.hat = n/sum(uzorak)
  return(lambda.hat)
}
simulacija_8a(1000, 4)

# b) X~gama(2, beta) 
simulacija_8b <- function(n, alfa, beta) {
  uzorak = rgamma(n, shape = alfa, rate = beta)
  beta.hat = 2*n/sum(uzorak)
  return(beta.hat)
}
simulacija_8b(1000, 2, 3)

# Zadatak 10
# a) X~U[0, theta]
simulacija_10a <- function(n, theta) {
 uzorak = runif(n, 0, theta)
 theta.hat = max(uzorak)
 return(theta.hat)
}
simulacija_10a(1000, 2)

# b) X~U[-theta, theta]
simulacija_10b <- function(n, theta) {
  uzorak = runif(n, -theta, theta)
  theta.hat = (-1)*min(uzorak)
  return(theta.hat)
}
simulacija_10b(1000, 2)


# Zadatak 11 
# a) X~N(m, theta^2)

simulacija_11a <- function(n, m, theta) {
  uzorak = rnorm(n, m, theta)
  m.hat = mean(uzorak)
  theta.hat = sqrt(var(uzorak)) # ili sd(uzorak) sto je koren iz var a var je Sn^2 tilda sto je popravljena uzoracka disperzjia
  return(c(m.hat, theta.hat))
}
simulacija_11a(1000, 1, 4)

# b) X~U[a, b]
simulacija_11b <- function(n, a, b) {
  uzorak = runif(n, a, b)
  a.hat = min(uzorak)
  b.hat = max(uzorak)
  return(c(a.hat, b.hat))
}
simulacija_11b(1000, 2, 3)


