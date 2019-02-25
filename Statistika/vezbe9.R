# Statistika - Vezbe 9
# asistent: Tamara Milic 
# Zorana Gajic 

# KOLMOGOROV-SMIRNOV test

# Zadatak 1 
# Imamo uzorak x i treba ispitati da li je uzorak iz Exp(0.2) raspodele.
# H0: F(x) = F0(x)  protiv H1: F(x) != F0(x)
x<-c(1.2,3.1,5.1,6.7)
n = length(x)
alfa = 0.05
F_0 = pexp(x, 0.2) # F_0(x) na osnovu fje raspodele 
F_n = ecdf(x) # Uvek empirijska
Dn = max(c(abs(F_0-F_n(x)), abs(F_0-c(0, F_n(x)[-n])))) # pogledati svesku i bice jasno
c<-0.624 # rucno iz kolmogorov-smirnov tabele
Dn > c 
# Dobijamo FALSE sto znaci da nismo upali u kriticnu sekciju => prihvatamo

# ili koristimo gotov test 
ks.test(x, "pexp", 0.2)
p.value = 0.8782 # iz testa vidimo
p.value < alfa
# Dobijamo FALSE, sto znaci da je p vece od alfa odnosno da nije u kriticnoj sekciji => 
# prihvatamo H0

# Zadatak 2 
# Dat je uzorak trenutaka otkaza masine u casovima. Sa pragom znacajnosti alfa = 0.01 
# ispitati da li trenutak otkaza ima uniformnu U[0,100] raspodelu.

# uzorak:
# 0-20  |  20-40  | 40-60  | 60-80  |  80-100
#  2        8         13      17        10

# U ovom zadatku nemamo predstavnike vec broj pojavljivanja u nekom intervalu
# => za predstavnike cemo uzeti sredine intervala
# Odnosno
x <- c(10, 30, 50, 70, 90)
n = length(x)
alfa = 0.01
F_n = c(1/50, 6/50, 16.5/50, 31.5/50, 45/50) # rucno 
F_0 = punif(x, 0, 100)
Dn = max(abs(F_n - F_0))
c = 0.226 # iz tabele
Dn > c
# Dobijamo FALSE, sto znaci da je p vece od alfa odnosno da nije u kriticnoj sekciji => 
# prihvatamo H0


# Zadatak 3
# Ispitati sa pragom znacajnosti 0.05 da li je uzorak 
x = c(0.3, 0.7, 0.9, 1.2, 1.4, 1.5, 1.6, 1.9, 2.0, 2.1, 2.3, 2.5, 2.6, 2.7, 3.0, 3.8, 3.9, 4.0) 
# iz normalne N(2,1) raspodele.
alfa = 0.05
n = length(x)
F_0 = pnorm(x, mean=2, sd = 1)
F_n = ecdf(x)
Dn = max(c(abs(F_0-F_n(x)), abs(F_0-c(0, F_n(x)[-n]))))
c = 0.309 # iz tabele

Dn > c
# Dobijamo FALSE, sto znaci da je p vece od alfa odnosno da nije u kriticnoj sekciji => 
# prihvatamo H0

# ili 
ks.test(x, "pnorm", 2, 1)

# Zadatak 4 
# Neka je uzorak 1.6, 0.6, 2.9, 1.7, 3.2, iz raspodele sa funkcijom raspodele F1 i 
# uzorak 3.8, 2.1, 3.0, 2.3, 3.1, 3.5 iz raspodele sa funkcijom raspodele F2. 
# Sa pragom znacajnosti 0.05 testirati H0:F1(x)=F2(x) protiv H1(F1(x)>F2(x)).
x<-c(1.6, 0.6, 2.9, 1.7, 3.2)
y<-c(3.8, 2.1, 3.0, 2.3, 3.1, 3.5)
sort(x)
sort(y)
ks.test(x, y)
ks.test(x,y, alternative = "greater")
p.value = 0.1403
p.value < 0.05
# FALSE => p.value > alfa => ne upada u kriticnu sekciju => prihvatamo H0


# Bojanin zadatak 1 - Sign test
# Procenat kofeina u kafi. Smatra se da je kafa dobro osusena ako je procenat me = 3.55
# Imamo uzorak
x = c(4.8, 4.0, 3.8, 4.3, 3.9, 4.6, 3.1, 3.7)
n = length(x)
# H1: me(x) != me
alfa = 0.05
me = 3.55

Tc = abs(sum(x < 3.55) - 8/2)
# Pogledati u svesci
# p.value = P{|Tc|>|Tc^|} = P{Tc > 3} + P{Tc < -3} = P{T-8/2 > 3} + P{T-8/2 < -3} .. = P{T=8}+P{T=0}
#                 (realiz)
p.value = dbinom(8, size=8, prob=1/2)+dbinom(0,8,1/2)
p.value < alfa
# TRUE => Odbacujemo H0


# Bojanin zadatak 2 - Vilkoksonov test
# Zavisnost od nekog leka. Da li je neki lek uticao.
# Pre koriscenja leka je nivo zavisnosti bio 28
# H0: m = 28 i H1: m>28
# => W = {T > c}

x = c(51, 53, 43, 36, 55, 55, 39, 43, 45, 27, 21, 26, 22, 43)
wilcox.test(x)

# Bojanin zadatak 3 - test nezavisnosti
mat = matrix(nrow = 2, ncol = 2, data = c(168, 73, 42, 26), byrow = T)
mat = as.table(mat)
dimnames(mat) = list(vk = c("da", "ne"), proizvodnja = c("da", "ne"))
chisq.test(mat)


