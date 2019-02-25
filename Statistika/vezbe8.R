# Statistika - Vezbe 8
# asistent: Tamara Milic 
# Zorana Gajic 
# U svim zadacim je kriticna sekcija W = {T > c}, gde je T test statistika

# Zadatak 1 
# Baca se kockica za igru. Testirati hipotezu da je kockica homogena.

# Simuliramo kockicku
uzorak <- sample(6, 60, replace = T)
M <- table(uzorak) # dobili smo tabelu oblika bas one koje nam treba

# i sad po formuli test statistike trazimo sve redom
r = length(M) # broj kategorija
np = length(uzorak)*1/6 # za svaku kocku verovatnoca da padne je 1/6 odnosno pk=1/6 za k={1..6}
# za svako i N*pi>5 (u ovom slucaju trivijalno, inace moramo da proveravamo)
# jer kad nije moramo spajati kategorije

# Po formuli test statistike
chi.0 = sum(((M - np)^2)/np)
alfa = 0.05 
c = qchisq(1-alfa,r-1)
chi.0 > c
# dobili smo FALSE, sto znaci da nismo upali u kriticnu => prihvatamo H0

# Zadatak 2 
# Dat je uzorak
#  [0, 1]  |  (1, 2]  |  (2, 3]  |  (3, 4]
#    23    |    26    |    23    |    28

# Sa pragom znacajnosti alfa=0.02 testirati hipotezu da X ima U[0,4] raspodelu. 
# Naci p-vrednost testa.

M = c(23, 26, 23, 28)
r = length(M)
N = sum(M)

np = N*1/4 # > 5
chi.0 = sum(((M - np)^2)/np)

alfa = 0.02
c = qchisq(1-alfa, r-1)
chi.0 > c
# dobili smo FALSE, sto znaci da nismo upali u kriticnu => prihvatamo H0

# p-vrednost testa 
p.value = 1 - pchisq(chi.0,r-1)
p.value < alfa 
# dobili smo FALSE => prihvatamo H0

# Zadatak 3
# Testirati da li X ima G(1/2) raspodelu za alfa = 0.05
#     1    |     2    |     3    |    4   |   5    |   6
#    45    |    30    |    15    |    6   |   2    |   2

# moramo dodati kategoriju zbog domena geometrijske raspodele
#     1    |     2    |     3    |    4   |   5    |   6  | >=6
#    45    |    30    |    15    |    6   |   2    |   2  |  0

M = c(45,30,15,6,2,2,0)
N = sum(M)
p = 1/2
pk <- c(dgeom(0:5, p), 1-pgeom(5, 0.5))
N*pk
# Vidimo da nam su nam poslednje tri kategorije manje od pet => spajamo poslednje tri kategorije


#     1    |     2    |     3    |    4   |  >=4
#    45    |    30    |    15    |    6   |  4
M<-c(45,30,15,6,4)
r = length(M)
pk = c(pk[1:4], sum(pk[5:7]))
np <- N*pk
# Sad vazi npi > 5 za svako i
chi.0 = sum((M-np)^2/np)
alfa = 0.05

c = qchisq(1-alfa, r-1)
chi.0 > c
# dobili smo FALSE, sto znaci da nismo upali u kriticnu => prihvatamo H0


# Zadatak 4
#  [0, 1]  |  (1, 2]  |  (2, 3]  |  (3, 4]
#    42    |    34    |    16    |    8
# Sa pragom znacajnosti 0.01 testirati da je X ~ Exp(lambda)

# Zbog domena eksponencijalne dodajemo kategoriju
#  [0, 1]  |  (1, 2]  |  (2, 3]  |  (3, 4] | (4, +besk)
#    42    |    34    |    16    |    8    |     0
M = c(42,34,16,8)
N = sum(M)

x.sr<-sum(c(0.5,1.5,2.5,3.5)*M)/N
# Posto je lambda nepoznato => ocenili smo ga metodom maksimalne verodostojnosti i dobili
lambda.hat = 1/x.sr

pk<-pexp(1:4,lambda.hat)-pexp(0:3,lambda.hat)
pk<-c(pk,1-pexp(4,lambda.hat))

M<-c(M,0)
np<-N*pk

r = length(M)
alpha<-0.05
c<-qchisq(1-alpha,r-1-1)
chi.0 <- sum((M - np) ^ 2 / np)
chi.0>c

# dobili smo TRUE => upali smo u kriticnu sekciju => odbacujemo H0


# Bojanin zadatak - 1
# U toku od 67 dana belezen je broj telefonskih poziva na jednoj liniji u vremenu od 10h
# Dobili smo:

#      0     1     2     3     4     5     6     7     8     9     10  
# Mi   3     7    21    11     6    15     1     0     0     1     2 
# npi 3.34 10.08 15.01 15.01 11.26 6.75  3.38  1.45  0.54   0.18  0.05

# Testirati H0 da obelezje X ima P(3)
# Dodajemo kategoriju zbog domena Puasonove raspodele
#      0     1     2     3     4     5     6     7     8     9     10  >10
# Mi   3     7    21    11     6    15     1     0     0     1     2     0
# npi 3.34 10.08 15.01 15.01 11.26 6.75  3.38  1.45  0.54   0.18  0.05  pribl 0

# moramo grupisati jer je negde npi<5
#          0         1     2     3     4     5     
# Mi      10        21    11     6    15     4
# npi 3.34+10.08  15.01 15.01 11.26 6.75  3.38+1.45+0.54+0.18+0.05=5.60 

M = c(10,21,11,6,15,4)
N = sum(M)
npi = c(14.14, 15.01, 15.01, 11.26, 6.75, 5.60)

r = length(M)
chi.0 = sum((M-npi)^2/np)
alfa = 0.05
c = qchisq(1-alfa, r-1)
chi.0 > c
# Dobili smo TRUE => Upali smo u kriticnu sekciju => odbaci H0


