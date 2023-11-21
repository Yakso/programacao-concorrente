#exemplo de uso de pool de processos em Python

from multiprocessing.pool import Pool
import time
import sys


#funcao que retorna 1 se n for primo e 0 caso contrario
def ehPrimo(n):
    if n<=1: return 0
    if n==2: return 1
    if n%2==0: return 0
    for i in range(3, int(n**(1/2)+1), 2):
        if n%i==0: return 0
    return 1

#funçao para testar a corretude do programa
def ehPrimoSequencial(n):
    soma = 0
    for i in range(1,n):
        soma += ehPrimo(i)
    return soma


if __name__ == '__main__':

    if len(sys.argv) == 1:
        print("Escreva o número de interesse como argumento")
        exit()
    try:
        N = int(sys.argv[1])
        if N <= 0:
            raise Exception
    except:
        print("Argumento deve ser um inteiro positivo maior igual a 1")
        exit()

    start = time.time()
    pool = Pool() #por default, cria um processo distinto para cada processador da maquina

    numbers = list(range(N))
    #map aceita uma funcao e um objeto iteravel, o pool pega cada valor do objeto iteravel e passa para um processo disponivel no poll que aplica a funcao sobre esse valor
    results = pool.map(ehPrimo, numbers)
    #soma os resultados
    n_primos = sum(results)
    if ehPrimoSequencial(N) != n_primos:
        print('Erro: resultado incorreto')
    else:
        print('Resultado correto')
        print('Existem {} primos no intervalo de 1 a {}'.format(n_primos, N))
        end = time.time()
        print('work took {} seconds'.format(end - start))
