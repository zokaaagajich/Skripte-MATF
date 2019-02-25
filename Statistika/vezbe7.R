# Statistika - Vezbe 7
# asistent: Tamara Milic 
# Zorana Gajic 

# Zadatak 1 
# Naci 95%-tni interval poverenja za parametar "m", ako je nasa slucajna velicina 
# X~N(m, sigma^2). Poznato nam je sigma^2 i n - broj elemenata u prostom slucajnom uzorku

n=64
x_sr=118
beta=0.95
sigma_kv= 100 #- poznata vrednost

# Resenje: 
c = qnorm((1+beta)/2)
interval_1 = x_sr - c*sqrt(sigma_kv/n)
interval_2 = x_sr + c*sqrt(sigma_kv/n)
interval_poverenja = c(interval_1, interval_2)

# Zadatak 2
# Naci 95%-tni interval poverenja za m na osnovu uzorka obima n ako je X~N(m, sigma^2), 
# sigma^2 - nepoznato ako je
n=25
x_sr=2.6
ocena_sigma_kv=170.36
beta=0.95

c = qt((1+beta)/2, n-1)
interval_1 = x_sr - c*sqrt(ocena_sigma_kv/n)
interval_2 = x_sr + c*sqrt(ocena_sigma_kv/n)
interval = c(interval_1, interval_2)


# Zadatak 3
# Neka je dato: X~ Norm(m, sigma^2).
n=20
s_n_kv=21.12
# Naci dvostrani interval poverenja za parametar sigma^2 za 
beta=0.99

# Resenje
c1 = qchisq((1-beta)/2, n-1)
c2 = qchisq((1+beta)/2, n-1)
interval_1 = (n-1)*s_n_kv/c2
interval_2 = (n-1)*s_n_kv/c1
interval = c(interval_1, interval_2)

# gornji interval poverenja
c = qchisq(beta)
interval = c((n-1)*s_n_kv/c, 2/0) # ne znam kako infinity da dodam drugcije hahaha

# donji interval poverenja
c = qchisq(1-beta)
interval = c(0, (n-1)*s_n_kv/c)


# Bojanin zadatak-1
# Precnik kuglice ima N(m, sigma^2) raspodelu za n=12 kuglica.
kuglice = c(3.02, 3.07, 2.98, 3.00, 3.01, 2.98, 2.98, 2.98, 3.00, 3.06, 2.97, 3.10)
# Naci 90% interval poverenja za srednju vrednost precnika kuglice

# sigma^2 je nepoznato
n = length(kuglice)
beta = 0.90
c = qt((1+beta)/2, n-1)

xn = mean(kuglice)
sn = sd(kuglice)
interval = c(xn-c*sn/sqrt(n), xn+c*sn/sqrt(n))

# Bojanin zadatak-2 
podaci = c(45, 43, 37, 41, 41, 60, 58, 61, 58, 60, 60, 61, 58)
n = length(podaci)
beta = 0.95

c = qt((1+beta)/2, n-1)
xn = mean(podaci)
sn = sd(podaci)
interval = c(xn-c*sn/sqrt(n), xn+c*sn/sqrt(n))

# Bojanin zadatak-3
# Imamo X~N(m, 16) 
n = 64
xn = 5
# Naci beta ako je m iz (4, 6) => duzina intervala = 2
# Im = (xn - c*sigma/sqrt(n), xn + c*sigma/sqrt(n))
# c=?
# duzina intervala = 2*c*sigma/sqrt(n) = 2 => c = 2
# 2 = c = qnorm((1+beta)/2) => (1+beta)/2 = pnorm(2)
pom = pnorm(2)
beta = 2*pom - 1


# Zadatak 4
# Od 500 ljudi, 285 se izjasnilo protiv. Naci 90% interval poverenja za verovatnocu 
# da se slucajno odabrana osoba izjasni protiv.
beta = 0.90
c = qnorm((1+beta)/2)
p = 285/500
# mozemo da aproksimiramo jer je n=500 > 100 i p~0.57
# => mozemo p(1-p) da zamenimo sa xn(1-xn)

n = 500
x_sr = 285/500
interval = c(xn-c*sqrt(x_sr*(1-x_sr))/sqrt(n), xn+c*sqrt(x_sr*(1-x_sr))/sqrt(n))


# Zadatak 5 
# Istrazivac procenjuje procenat gojaznih osoba sa visokim krvnim pritiskom.
# Od 25 ispitanika - 20 ima visok pritisak.
# Naci 95% interval poverenja za parametar p, koji predstavlja verovatnocu 
# da gojazna osoba ima visok krvni pritisak.
n = 25
xn = 20/25
beta = 0.95
# ne mozemo aproksimaciju jer je n < 100

c = qnorm((1+beta)/2)
# MORA RUCNO KVADRATNA JEDNACINA ....
p1 = 0.63
p2 = 0.90
interval = c(p1,p2)

# Zadatak 6
# Naci 90% i 80% interval poverenja za istrazivanje u kojem je 
# ucestvovalo 100 ljudi
# i srednja vrednost je 0.45.
n = 100
xn = 0.45
# mozemo koristiti aproksimaciju

beta = 0.90
c = qnorm((1+beta)/2)
interval = c(xn-c*sqrt(xn*(1-xn))/sqrt(n), xn+c*sqrt(xn*(1-xn))/sqrt(n))

beta = 0.80
c = qnorm((1+beta)/2)
interval = c(xn-c*sqrt(xn*(1-xn))/sqrt(n), xn+c*sqrt(xn*(1-xn))/sqrt(n))

# Zadatak 7
# Student zeli da istrazi procenat levorukih osoba. 
# Ispitano je 100 ljudi od kojih je samo 5 levorukih.
# Ako se nadje 90% interval poverenja da li ce on sadrzati vrednost 0.1? 
# Ako ne - koliko-procentni interval poverenja treba da trazimo?
n = 100
xn = 5/100
beta = 0.90
# Da li 0.1 pripada (a,b) , ako P(a<=p<=b)=0.9?

c = qnorm((1+beta)/2)
# RUCNO
p1 = 0.025
p2 = 0.099
interval = c(p1, p2)
# => ne upada 0.1

# ali za beta = 0.91 vec upada jer je c tada
c = qnorm((1+0.91)/2) # 1.69
# i interval je (0.024, 0.10001) otprilike.

# Zadatak 8
# Od poslednjih 10 puta kada je tostirani sendvic pao - devet puta 
# je pao na "ukusnu stranu".
# Ako je uzorak iz Bernulijeve raspodele sa parametrom p, 
# naci 80% interval poverenja da sendvic pada na "ukusnu stranu".
n = 10
xn = 9/10 
beta = 0.8
c = qnorm((1+beta)/2)
# RUCNO
p1 = 0.72
p2 = 0.97
interval = c(p1, p2)

# Zadatak 9
# Izvrseno je ispitivanje 250 ljudi, ocenjeno p odgovora je 0.45.
# Isto istrazivanje je ponovljeno na 1000 ljudi i dobijen isti odnos.
# Naci 95% interval poverenja u oba slucaja. Koliko puta je drugi manji od prvog?
# Koliko veliko treba da bude n u istrazivanju da bi 95% interval poverenja 
# bio duzine ne vece od 0.01.

duzina_intervala <- function(xn, beta, n) {
  c = qnorm((1+beta)/2)
  return(2*c*sqrt(xn*(1-xn)/n))
}

d1 = duzina_intervala(0.45, 0.95, 250)
d2 = duzina_intervala(0.45, 0.95, 1000)
d1/d2

# Koliko veliko treba da bude n u istrazivanju da bi 95% interval poverenja 
# bio duzine ne vece od 0.01.
# d = 2*c*sqrt(xn*(1-xn)/n) i d<=0.01
xn = 0.45
c = qnorm((1+0.95)/2)
n = (2*c*sqrt(xn*(1-xn))/0.01)^2


# Zadatak 10
# X iz Bernulijeve sa parametrom p. Dobijen PSU duzine n=250
# Apriorna za p je Beta(2, 3).
# Tada aposteriorna je Beta(sum(x)+2, n - sum(x)+3)
# Naci interval prekrivanja za beta= 0.9

# Generisemo Bernulijev uzorak
x = sample(x = c(0,1), size = 250, prob=c(0.45, 0.55), replace=TRUE)
suma = sum(x)

a = qbeta((1-beta)/2, suma + 2, 250-suma+3) 
b = qbeta((1+beta)/2, suma + 2, 250-suma+3)
interval_prekrivanja = c(a, b)


# Bojanin zadatak - 4
# U 200 anketiranih ljudi 70 se slaze sa predlogom vlade. Naci 95% interval poverenja
# za one koji se slazu sa predlogom vlade.

n = 200
xn = 70/200
# n > 100 i xn nije blizu 0 i 1 => mozemo aproksimirati p(1-p) sa xn(1-xn)
beta = 0.95
c = qnorm((1+beta)/2)
interval_poverenja = c(xn - c*sqrt(xn*(1-xn)/n), xn + c*sqrt(xn*(1-xn)/n))




