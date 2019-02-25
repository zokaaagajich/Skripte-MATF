# Statistika - Vezbe 6
# asistent: Tamara Milic 
# Zorana Gajic 

install.packages("BSDA")
library(BSDA)

# Zadatak 1 - Pretpostavimo da imamo uzorak iz N(m, 10). Testirati koristeci p-vrednost testa 
# hipotezu da H0: m = 2 i H1: m>2

x = c(5.6884562,  2.0308148, -0.2508697, -2.5062107,  5.1422191,  1.1849337,  3.6343267,
  4.6528487,  1.5760364,  5.1431170,  1.8864374,  4.4475159,  3.5619849,  4.7377123,
  -1.2994996,  2.4134766,  0.2744983,  5.2431267, -0.1152874,  0.8769407,  2.1222095,
  1.8561701,  3.0470218,  0.4738757,  2.1079643,  2.5249362, -2.2613439,  1.0116844,
  2.1796532,  1.6629011,  0.6026096,  3.0568876,  0.1365397, -5.0585758,  2.3148987,
  4.0924715, -0.9189906,  7.6984261, -1.9539183,  2.7631545,  5.5492733, -1.5838495,
  -2.3246922,  0.7752239,  1.3790237, -1.1731306,  4.8165865,  2.9824243,  6.0295380,
  0.1998773,  9.0725547, -1.2977859,  1.9417684, -3.5287460,  2.5369642, -1.3349065,
  -1.7820453,  4.2251318,  2.9737008,  1.8158107)
z.test(x, sigma.x=sqrt(10), mu=2, alternative = "greater")
# U rezultatu dobijemo da je p-value = 0.7014, sto je > alfa = 0.05 => prihvatamo H0

# Kako bismo rucno uradili ovaj zadatak?
n = length(x)
srednja_vr_uzorka = mean(x)
odstupanje = sqrt(10)

t_kapa_1 = (srednja_vr_uzorka - 2)/odstupanje * sqrt(n)

alfa = 0.05
c = qnorm(1-alfa)

t_kapa_1 > c
# Dobili smo FALSE, sto znaci da t_kapa ne upada u kriticnu oblast => prihvatamo H0

# Da je u zadatku H1: m != 2
c = qnorm(1-alfa/2)
abs(t_kapa_1) > c


# Moc testa
nadji_moc <- function(c, donja_granica, gornja_granica, korak) {
  t = seq(donja_granica, gornja_granica, korak)
  rezultat = 1 - pnorm(c + (2-t)/sqrt(10)*sqrt(60))
  return(rezultat)
}

alfa = 0.05
c = qnorm(1-alfa)
nadji_moc(c, 2, 3.5, 0.01)


# Zadatak 2 
# Prodavac smatra da, ako je prosecna zarada 2$ po osobi, ostvaruje se dobar profit i da treba otvoriti prodavnicu. 
# Profit ima normalnu N(m,(1.7)^2) raspodelu. Prodavac je uzeo uzorak od 25 slucajno odabranih osoba i dobio 
# prosecnu vrednost profita 2.842$.
# Ako je alfa=0.05, da li se na osnovu dobijenog rezultata moze zakljuciti da treba otvoriti prodavnicu?
# Sta bi ovde bila nulta hipoteza a sta alternativna?

# H0: m=2, dok je H1: m>2
# Racunamo c 
alfa = 0.05
c = qnorm(1-alfa)
t_kapa_2 = (2.842-2)/(1.7)*sqrt(25)
t_kapa_2 > c
# Dobili smo da je t_kapa vece od c odnosno da je upao u kriticnu oblast => H0 ne prihvatamo!

# Zadatak 3
# Posmatrajmo uzorak generisan u zadatku 1. Takodje ovoga puta cemo pretpostaviti da nam nije poznato σ^2. 
# Koristeci p-vrednost testa testirati H0:m=2, protiv H1:m≠2.
t.test(x, mu=2, alternative="two.sided")
# p-value = 0.5524 > alfa = 0.05 => prihvatamo H0

# rucno
n = length(x)
alfa = 0.05
c = qt(1-alfa/2, n)
t_kapa_3 = (mean(x)-2)/sqrt(var(x))*sqrt(n) 
p = 2 * min(pt(t_kapa_3, n-1), 1-pt(t_kapa_3, n-1)) 
#pt - vrednost funkcije raspodele studentove slucajne velicine u tacki 
# t_kapa sa n-1 stepeni slobode.
p

# Zadatak 4
# Za stare teniske loptice imamo: X~N(28,0.25)
# Proizvedena je nova serija teniskih loptica. Da li je smanjena disperzija? 
# Na osnovu uzorka od n=25 loptica imamo:

n = 25
alfa = 0.05
S_tilda_kv_4 = 0.4197

# Postaviti hipoteze i testirati da li je disperzija smanjena
# H0: sigma^2 = 0.25 i H1: sigma^2 < 0.25
t_kapa_4 = (n-1)*S_tilda_kv_4/0.25
c = qchisq(alfa, n-1)
t_kapa_4 < c
# dobijamo FALSE sto znaci da je t_kapa > c => nije upao u kriticnu oblast => H0 prihvatamo


# Zadatak 5
# Menadzer neke firme hoce da vidi da li je za firmu bolje da radi od 9:30 do 17:30(a) ili od 10 do 18(b). 
# Uzorak je 25 slucajno odabranih dana kada je firma radila po prvom i 25 dana po drugom rezimu i dobio je sledece 
# rezultate:

# Prvi rezim: prosecna zarada je ma=570, sa uzorackim disperzijama (Sa)^2=1600, i
# Drugi rezim: mb=600, (Sb)^2=625
# Sa nivoom znacajnosti testa od 0.05 testirati da li se veca zarada ostvaruje u drugom rezimu rada.

# Mi u zadatku hocemo da testiramo sta? 
# H0: ma=mb a H1: ma<mb

# Prvo moramo da proverimo disperzije.
# H0: sigma1^2 = sigma2^2 a H1: sigma1^2 != sigma2^2
alfa = 0.1
# Koristimo Fiserovu

f_kapa5 = 1600/625

c1 = qf(alfa/2, n-1,n-1)
c2 = qf(1-alfa/2, n-1, n-1)
f_kapa5 < c1 | f_kapa5 > c2
# Dobili smo true, sto znaci da je f_kapa upala u kriticnu sekciju => odbacujemo H0
# sledi imamo slucaj kada disperzije nisu jednake

Q = (1600 / 25 + 625/25) / ((1600/25)^2 / 24 + (625/25)^2 / 24)  
# Q > 30 => N(0,1)
c = qnorm(0.05)
t_kapa_5 = (570-600)/sqrt(1600/25 + 625/25)
t_kapa_5 < c
# Odbacujemo H0 jer smo dobili true, sto znaci da je upalo u kriticnu oblast =>
# bolje da uzme rezim B

# Zadatak 6
# Izvrseno je istrazivanje da li je cena artikala u jednom hipermarketu veca od cene u drugom i slucajno je odabrano 
# 20 artikala i belezene su njihove razlike u supermarketima i dobijene su sledece vrednosti:
# D=0.045 - srednja vrednost razlika u ceni,
# (SD)^2=0.0084 ocenjena disperzija razlika u ceni.

# Sa nivoom znacajnosti testa alfa = 0.05 ispitati da li su cene u prvom hipermarketu znacajno vece od cena u drugom.


# H0: d0=0 dok je H1: d1>0

t_kapa_6 = 0.045*sqrt(20)/sqrt(0.0084)
n = 20
alfa = 0.05
c = qt(1-alfa, n-1)
t_kapa_6 > c
# Dobijamo TRUE => upali smo u kriticnu sekciju => odbacujemo H0
