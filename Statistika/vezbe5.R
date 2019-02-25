# Statistika - Vezbe 5
# asistent: Tamara Milic 
# Zorana Gajic 


# Zadatak 1
# Neka je X iz familije Bernulijevih raspodela sa parametrom θ i neka je apriorna raspodela za θ 
# β(2,3). Naci aposteriornu ocenu za θ.
zadatak1 <- function(theta, a, b, n) {
  # apriorna 
  curve(dbeta(x,a,b), 0, 1)
  
  par(new = T) # sluzi da mozemo i ovu sledecu funkciju da prikazemo na grafiku od pre
  # aposteriorna
  x1 <- sample(c(0,1), n, replace=TRUE, prob=c(1-theta, theta))
  curve(dbeta(x,sum(x1)+a,n-sum(x1)+b), 0, 1)
}
zadatak1(0.5,2,3,100)


# Zadatak 2
x<-2
q<-0.7*choose(3,2)*0.4^2*(1-0.4)/(0.7*choose(3,2)*0.4^2*(1-0.4)+0.3*choose(3,2)*0.5^2*(1-0.5))
q
1-q



# Zadatak 3
# uzorak je iz N(1,2), a apriorna je N(0,1) 

zadatak3 <- function(n, theta, sigma2, mi, tau2) {
 # apriorna
 curve(dnorm(x, mi, tau2), -5, 5, col="blue")
 
 par(new = TRUE)
 # aposteriorna
 x1 <- rnorm(n, theta, sigma2)
 rho<-n/sigma2+1/tau2
 curve(dnorm(x, 1/rho*(n*mean(x1)/sigma2+mi/tau2), 1/rho), -5, 5, col="red")
  
}
zadatak3(50, 1, 2, 0, 1)


# Zadatak 5
zadatak5 <- function(n, p) {
  # apriorna 
  curve(dunif(x, 0, 1), 0, 1, col="blue")
  
  par(new = TRUE)
  # aposteriorna
  x1 <- sample(c(0,1), n, prob=c(1-p, p), replace=TRUE)
  curve(dbeta(x, sum(x1)+1, n-sum(x1)+1), 0, 1, col="red")
} 
zadatak5(50, 1/2)

p.hat = (sum(x1)+1)/(n+2)
p.hat


# Zadatak 6 
zadatak6 <- function(n, p, a, b) {
  # apriorna
  curve(dbeta(x, a, b), 0, 1, col="blue")
  
  par(new = TRUE)
  # aposteriorna
  x1 <- rgeom(n, p)+1 #uvek +1 zbog implementacije rgeom
  curve(dbeta(x, n+a, sum(x1)-n+b), 0, 1, col="red")
}
zadatak6(50, 0.8, 2, 3)

theta.hat = (n+a)/(sum(x1)+a+b)