


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void menu() ;
void codificar() ;
void descodificar() ;


int main()
{

	printf("HC: Algoritmo de cifrado por HiperCurva v0.1\n") ;
	printf("HC 1.0 96/16/1S\n\n" ) ;

	while (1) menu() ;
	
	printf("ERROR: Terminación anormal del programa" ) ;

	return 1 ;
}

void menu() 
{
	int op ;
	printf("\n\n\tMENU PRINCIPAL\n\n" ) ;
	printf("1: Codificar fichero\n" ) ;
	printf("2: Descodificar fichero\n" ) ;
	printf("\n0: Salir\n\n" ) ;
	printf("Opcion: " ) ;
	scanf("%d" , &op ) ;
	switch (op)
	{
	case 0:
		exit(0) ;
		break ;
	case 1:
		codificar() ;
		break ;
	case 2:
		descodificar() ;
		break ;
	default:
		break ;
	}
}

void codificar() 
{
	char clave[12] ;
	char clavet[12] ;
	char claved[6][4] ;
	int *pool ;
	char *poolc ;
	char *poole ;
	int *tabla_sizes ;
	int *tabla ;
	char fileclaro[256];
	char fileclave[256];
	char filecode[256];
	FILE *fclaro ;
	fpos_t fsize ;
	FILE *fclave ;
	FILE *fcode ;
	size_t max_bloques ;
	int num_bloques ;
	int i1, i2 , i3 , i4 , i5 , i6 , i7 ;
	int v1, v2 , v3 , v4 , v5 , v6 ;
	int v[10] ;
	long acumulado ;
	int size_bloque ;
	int num_bloque ;
	int size_dim[6] ;
	int size_poolc ;
	int dir_dim ;

	printf("\n\n\n\tCODIFICAR FICHERO\n\n" ) ;
	printf("Introduzca ruta y nombre de archivo del fichero a codificar\n::" ) ;
	scanf("%s" , &fileclaro ) ;
	printf("\n\nIntroduzca ruta y nombre de archivo de clave\n::" ) ;
	scanf("%s" , &fileclave ) ;
	printf("\n\nIntroduzca ruta y nombre de archivo a generar\n::" ) ;
	scanf("%s" , &filecode ) ;

	// Iniciamos el proceso

	// Abrimos el fichero de claves
	fclave = fopen( &fileclave , "rb" ) ;
	fread( &clave , 12 , 1 , fclave ) ;
	fclose( fclave ) ;
    
	//printf("\n%d %d %d %d %d %d %d %d %d %d %d %d\n" , clave[0] ,clave[1] ,clave[2] ,clave[3] ,clave[4] ,clave[5] ,clave[6] ,clave[7] ,clave[8] ,clave[9] ,clave[10] ,clave[11] ) ;

	// Abrimos el fichero claro
	fclaro = fopen( &fileclaro , "rb" ) ;

	// Obtenemos el tamaño del fichero
	fseek( fclaro , 0 , SEEK_END ) ;
	fgetpos( fclaro , &fsize ) ;
	fseek( fclaro , 0 , SEEK_SET ) ;

	// Ajustamos la clave al sistema de alternación +/-
	for ( i1 = 0 ; i1 < 12 ; i1++ )
		if ( clave[i1]&1 )
			clave[i1] = (int) clave[i1]/2 ;
		else
			clave[i1] = (int)( - (clave[i1]/2)) ;


	// El tamaño mínimo de bloque es de 0xFFF, de modo que reservamos una tabla
	// de fsize/0xFFF
	max_bloques = (int)((fsize+0xFFF) / 0xFFF ) ;
	tabla_sizes = calloc( max_bloques , sizeof(int) ) ;
	
	// Generamos la curva 1D para obtener los tamaños de bloque
	acumulado = 0 ;
	i1 = 1 ;
	
	printf("\n\nTAMAÑO DE BLOQUES:" ) ;
	while (1)
	{
		v1 = clave[0]*i1 + clave[1]*(i1*i1) + clave[2]*(i1*i1*i1) + clave[3]*(i1*i1*i1*i1) ;
		v1 = v1%0xFFFF ;
		if ( v1 < 0 ) v1 = 0xFFFF-v1 ;
		if ( v1 < 0xFFF ) v1 = 0xFFF ;
		acumulado+= v1 ;
		if ( acumulado >= fsize )
		{
			v1 -= acumulado - fsize ;
			tabla_sizes[i1-1] = v1 ;
			printf("\n%d" , tabla_sizes[i1-1] ) ;
			break ;
		}
		tabla_sizes[i1-1] = v1 ;
		printf("\n%d" , tabla_sizes[i1-1] ) ;
		i1++ ;
	}    	
	
	num_bloques = i1 ;
	
	
	    
	printf("\n\nMaximo numero de bloques para el fichero: %d" , max_bloques ) ;
	v1 = fsize / ((0xFF + 0xF) / 2 ) ;
	printf("\nEstimacion media de bloques para el fichero: %d" , v1 ) ;
	printf("\nNumero de bloques para el fichero: %d" , num_bloques ) ;


	// Ahora ordenamos los bloques con otra curva 1D
	pool = calloc( 0xFFFF * 2 , sizeof(int) ) ;

	//i1 = 1 ;

	printf("\n\nCOORDENADAS PARA ORDENACION DE BLOQUES:") ;

	for ( i1 = 1 ; i1 <= num_bloques ; i1++ )
	{
		v1 = clave[4]*i1 + clave[5]*(i1*i1) + clave[6]*(i1*i1*i1) + clave[7]*(i1*i1*i1*i1) ;
		v1 = v1%0xFFFF ;
		if ( v1 < 0 ) v1 = 0xFFFF+v1 ;
		if ( pool[v1*2] )
			//Busqueda positiva !!!
			while ( pool[v1*2] )
			{
				v1++ ;
				v1%0xFFFF ;
			}
		pool[(v1*2)+0] = 1 ;
		pool[(v1*2)+1] = i1 ;
		printf("\n%d %d" , pool[(v1*2)+1], v1 ) ;
	}    	

	printf("\n\nORDEN DE BLOQUES:\n" ) ;

	tabla = calloc( num_bloques , sizeof(int) ) ;

	i2 = 0 ;

	// Hace falta hacer un sistema para que no siempre suela empezar
	// por el bloque 1
	for ( i1 = 0 ; i1 < 0xFFFF ; i1++ )
	{
		if ( pool[i1*2] )
		{
			tabla[ i2++ ] = pool[(i1*2)+1] ;
			printf("\n%d" , pool[(i1*2)+1] ) ;
		}
	}

	// Liberamos un poco de basura
	free(pool) ;
	
	// Generamos el fichero de salida
	fcode = fopen(filecode , "w+b") ;
    
	// Preparamos clavet
	for ( i1 = 0 ; i1 < 12 ; i1++ )
		clavet[i1] = clave[i1] ;

	// Codificamos los bloques
	for ( i1 = 1 ; i1 <= num_bloques ; i1++ )
	{
		printf("\n\nCODIFICANDO BLOQUE %d" , i1 ) ;
		
		// Leemos el bloque en memoria
		num_bloque = tabla[i1-1] ;
		size_bloque = tabla_sizes[ num_bloque-1 ] ;
		poolc = calloc( size_bloque , 1 ) ;
		acumulado = 0 ;
		for ( i2 = 1 ; i2 < num_bloque ; i2++ )
			acumulado += tabla_sizes[ i2-1 ] ;
		printf("acumulado: %d\n" , acumulado ) ;
		fseek( fclaro , acumulado , SEEK_SET ) ;		
		fread( poolc , 1 , size_bloque , fclaro ) ;

		
		
		// Generamos la clave para el bloque con una curva 1D aplicada bit a bit
		pool = calloc( 0xFF * 1 , sizeof(int) ) ;
		
		for ( i2 = 1 ; i2 <= 12 ; i2++ )
            for ( i3 = 0 ; i3 < 8 ; i3++ )
			{
				v1 = clavet[8]*(i2) + clavet[9]*(i2*i2) + clavet[10]*(i2*i2*i2) + clavet[11]*(i2*i2*i2*i2) ;
				v1 = v1%0xFF ;
				if ( v1 < 0 ) v1 = 0xFF + v1 ;
				while ( pool[ v1 ]!=0 )
				{
					v1++ ;
					v1 = v1%0xFF ;
				}
				if ( clavet[i2-1] & ((char)(pow( 2 , i3 )) ) )
					pool[ v1 ] = 1 ;
				else
					pool[ v1 ] = 2 ;
			} ;

		// De nuevo habria que hacer algo para que el primer bit no se quedase fijo!!
		
		//printf("\n\nClave: " ) ;
		
		i4 = 0 ;
		for ( i2 = 0 ; i2 < 12 ; i2++ )
		{
			clavet[i2] = 0 ;
			for ( i3 = 0 ; i3 < 8 ; i3++ )
				while(1)
				{
					if ( pool[i4]!= 0 )
					{
						if ( pool[i4] == 1 )
                            clavet[i2] |= (int)pow(2,i3) ;
						//else
						//	clavet[i2] &= 0xFF - pow(2,i3) ;
							//exit(0) ;

						i4++ ;
						break ;
					}
					i4++ ;
				} ;
			//printf( "%i " , clavet[i2] ) ;
		} ;

		// Proceso de mutación mediante una curva 1D aplicada sobre si misma
		// Los cambios que tengan lugar durante la mutación afectarán a los
		// siguientes pasos de la mutacion!!!!!!
		for ( i2 = 0 ; i2 < 16 ; i2++ )
		{
			v1 = clavet[0]*(i2) + clavet[1]*(i2*i2) + clavet[2]*(i2*i2*i2*i2) ;
			v1 %= 96 ;
			if ( v1 < 0 ) v1 += 96 ;
			v2 = v1 / 8 ;
			v3 = v1 % 8 ;
			clavet[v2] |= (int)pow( 2 , v3 ) ;

			v1 = clavet[3]*(i2) + clavet[4]*(i2*i2) + clavet[5]*(i2*i2*i2*i2) ;
			v1 %= 96 ;
			if ( v1 < 0 ) v1 += 96 ;
			v2 = v1 / 8 ;
			v3 = v1 % 8 ;
			clavet[v2] &= 0xFF - (int)(pow( 2 , v3 )) ;
		}

        // Sacamos los valores mutados
		//printf("\nClave: " ) ;
		//for ( i2 = 0 ; i2 < 12 ; i2++ )
		//	printf("%d " , clavet[i2] ) ;

        // Liberamos memoria
		free(pool) ;

		// Ordenamos las dimensiones
		printf("\n") ;

		pool = calloc( 0xF , sizeof(int) ) ;

		for( i2 = 1 ; i2 <= 6 ; i2++ )
		{
			v1 = clavet[0]*(i2) + clavet[4]*(i2*i2) + clavet[8]*(i2*i2*i2) ;
			v1 %= 0xF ;
			if ( v1 < 0 ) v1 += 0xF ;
			while ( pool[v1] )
				v1 = (v1 +1) % 0xF ;
			pool[v1] = i2 ;
		}

		i3 = 0 ;
		for ( i2 = 0 ; i2 < 6 ; i2++ )
		{
			while( !pool[i3++] ) ;
			claved[i2][0] = (clavet[pool[i3-1]*2] & 0xF0)>>4 ;
			claved[i2][1] = (clavet[pool[i3-1]*2] & 0xF) ;
			claved[i2][2] = (clavet[(pool[i3-1]*2) + 1] & 0xF0)>>4 ;
			claved[i2][3] = (clavet[(pool[i3-1]*2) + 1] & 0xF) ;

			
			if ( claved[i2][0] % 2 )
				claved[i2][0] = -claved[i2][0] ;
			if ( claved[i2][1] % 2 )
				claved[i2][1] = -claved[i2][1] ;
			if ( claved[i2][2] % 2 )
				claved[i2][2] = -claved[i2][2] ;
			if ( claved[i2][3] % 2 )
				claved[i2][3] = -claved[i2][3] ;
			claved[i2][0] /= 2 ;
			claved[i2][1] /= 2 ;
			claved[i2][2] /= 2 ;
			claved[i2][3] /= 2 ;
			
			//printf("%d\t%d\t%d\t%d\n", claved[i2][0] , claved[i2][1] , claved[i2][2] , claved[i2][3] ) ;
		}

		free(pool) ;

		// Calculamos tamaños de cada dimension
		//printf("\n\n") ;
		for ( i2 = 1 ; i2 <= 6 ; i2++ )
		{
			v1 = clavet[1]*(i2) + clavet[5]*(i2*i2) + clavet[9]*(i2*i2*i2) ;
			v1 %= 4 ;
			if ( v1 < 0 ) v1 += 4 ;
			size_dim[i2-1] = v1 + 12 ;
			//printf("%d " , size_dim[i2-1] ) ;
		}
		
        // Generamos el espacio 6D
		size_poolc = size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3]*size_dim[4]*size_dim[5] ;
		poole = calloc( size_poolc * 2 , 1 ) ;

		// Generamos la curva 6D
		for ( i2 = 1 ; i2 <= size_bloque ; i2++ )
		{
			v1 = claved[0][0]*(i2) + claved[0][1]*(i2*i2) + claved[0][2]*(i2*i2*i2) + claved[0][3]*(i2*i2*i2*i2) ;
			v2 = claved[1][0]*(i2) + claved[1][1]*(i2*i2) + claved[1][2]*(i2*i2*i2) + claved[1][3]*(i2*i2*i2*i2) ;
			v3 = claved[2][0]*(i2) + claved[2][1]*(i2*i2) + claved[2][2]*(i2*i2*i2) + claved[2][3]*(i2*i2*i2*i2) ;
			v4 = claved[3][0]*(i2) + claved[3][1]*(i2*i2) + claved[3][2]*(i2*i2*i2) + claved[3][3]*(i2*i2*i2*i2) ;
			v5 = claved[4][0]*(i2) + claved[4][1]*(i2*i2) + claved[4][2]*(i2*i2*i2) + claved[4][3]*(i2*i2*i2*i2) ;
			v6 = claved[5][0]*(i2) + claved[5][1]*(i2*i2) + claved[5][2]*(i2*i2*i2) + claved[5][3]*(i2*i2*i2*i2) ;

            v1 %= size_dim[0] ;
			v2 %= size_dim[1] ;
			v3 %= size_dim[2] ;
			v4 %= size_dim[3] ;
			v5 %= size_dim[4] ;
			v6 %= size_dim[5] ;

			if ( v1 < 0 ) v1 += size_dim[0] ;
			if ( v2 < 0 ) v2 += size_dim[1] ;
			if ( v3 < 0 ) v3 += size_dim[2] ;
			if ( v4 < 0 ) v4 += size_dim[3] ;
			if ( v5 < 0 ) v5 += size_dim[4] ;
			if ( v6 < 0 ) v6 += size_dim[5] ;

			v[1] = v1 ;
			v[2] = v2 ;
			v[3] = v3 ;
			v[4] = v4 ;
			v[5] = v5 ;
			v[6] = v6 ;

			while ( poole[(v[1] + v[2]*size_dim[0] + v[3]*size_dim[0]*size_dim[1] + v[4]*size_dim[0]*size_dim[1]*size_dim[2] + v[5]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3] + v[6]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3]*size_dim[4]) *2 ] )
			{
				v[1] = (v[1] + 1) % size_dim[0] ;
				if ( v[1] == v1 )
				{
					v[2] = (v[2] + 1) % size_dim[1] ;
					if ( v[2] == v2 )
					{
						v[3] = (v[3] + 1) % size_dim[2] ;
						if ( v[3] == v3 )
						{
							v[4] = (v[4] + 1) % size_dim[3] ;
							if ( v[4] == v4 )
							{
								v[5] = (v[5] + 1) % size_dim[4] ;
								if ( v[5] == v5 )
									v[6] = (v[6] + 1) % size_dim[5] ;
							}
						}
					}
				}
			}
			//printf("A") ;
			dir_dim = (v[1] + v[2]*size_dim[0] + v[3]*size_dim[0]*size_dim[1] + v[4]*size_dim[0]*size_dim[1]*size_dim[2] + v[5]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3] + v[6]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3]*size_dim[4]) *2 ;
			poole[dir_dim] = 1 ;
			poole[dir_dim+1] = poolc[i2-1] ;

		}

		acumulado = 0 ;
		// Y ahora leemos la información
		for ( v[6] = 0 ; v[6] < size_dim[5] ; v[6]++ )
			for ( v[5] = 0 ; v[5] < size_dim[4] ; v[5]++ )
				for ( v[4] = 0 ; v[4] < size_dim[3] ; v[4]++ )
					for ( v[3] = 0 ; v[3] < size_dim[2] ; v[3]++ )
						for ( v[2] = 0 ; v[2] < size_dim[1] ; v[2]++ )
							for ( v[1] = 0 ; v[1] < size_dim[0] ; v[1]++ )
							{
								dir_dim = ( v[1] + v[2]*size_dim[0] + v[3]*size_dim[0]*size_dim[1] + v[4]*size_dim[0]*size_dim[1]*size_dim[2] + v[5]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3] + v[6]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3]*size_dim[4]) *2 ;
								if (poole[dir_dim])
								{
									acumulado++ ;
									//printf("\n%d %d %d %d %d %d : %d", v[1], v[2], v[3], v[4], v[5], v[6] , dir_dim) ;
									fwrite( &poole[dir_dim+1] , 1 , 1 , fcode ) ;
								}
							} ;
		printf("\nEscrito: %d", acumulado ) ;
		//fwrite( poolc , 1 , size_bloque , fcode ) ;
							


		printf("\n\n" ) ;



		// Liberamos memoria
		free( poole ) ;
		free( poolc ) ;
	}


	fclose( fclaro ) ;
	fclose( fcode ) ;
}


void descodificar() 
{
	char clave[12] ;
	char clavet[12] ;
	char claved[6][4] ;
	char *tabla_clave ;
	char *tabla_clavet ;
	int *pool ;
	char *poolc ;
	char *poole ;
	int *tabla_sizes ;
	int *tabla ;
	char fileclaro[256];
	char fileclave[256];
	char filecode[256];
	FILE *fclaro ;
	fpos_t fsize ;
	FILE *fclave ;
	FILE *fcode ;
	size_t max_bloques ;
	int num_bloques ;
	int i1, i2 , i3 , i4 , i5 , i6 , i7 ;
	int v1, v2 , v3 , v4 , v5 , v6 ;
	int v[10] ;
	long acumulado ;
	int size_bloque ;
	int num_bloque ;
	int size_dim[6] ;
	int size_poolc ;
	int dir_dim ;

	printf("\n\n\n\tDESCODIFICAR FICHERO\n\n" ) ;
	printf("Introduzca ruta y nombre de archivo del fichero a descodificar\n::" ) ;
	scanf("%s" , &fileclaro ) ;
	printf("\n\nIntroduzca ruta y nombre de archivo de clave\n::" ) ;
	scanf("%s" , &fileclave ) ;
	printf("\n\nIntroduzca ruta y nombre de archivo a generar\n::" ) ;
	scanf("%s" , &filecode ) ;

	// Iniciamos el proceso

	// Abrimos el fichero de claves
	fclave = fopen( &fileclave , "rb" ) ;
	fread( &clave , 12 , 1 , fclave ) ;
	fclose( fclave ) ;
    
	//printf("\n%d %d %d %d %d %d %d %d %d %d %d %d\n" , clave[0] ,clave[1] ,clave[2] ,clave[3] ,clave[4] ,clave[5] ,clave[6] ,clave[7] ,clave[8] ,clave[9] ,clave[10] ,clave[11] ) ;

	// Abrimos el fichero claro
	fclaro = fopen( &fileclaro , "rb" ) ;

	// Obtenemos el tamaño del fichero
	fseek( fclaro , 0 , SEEK_END ) ;
	fgetpos( fclaro , &fsize ) ;
	fseek( fclaro , 0 , SEEK_SET ) ;

	// Ajustamos la clave al sistema de alternación +/-
	for ( i1 = 0 ; i1 < 12 ; i1++ )
		if ( clave[i1]&1 )
			clave[i1] = (int) clave[i1]/2 ;
		else
			clave[i1] = (int)( - (clave[i1]/2)) ;


	// El tamaño mínimo de bloque es de 0xFFF, de modo que reservamos una tabla
	// de fsize/0xFFF
	max_bloques = (int)((fsize+0xFFF) / 0xFFF ) ;
	tabla_sizes = calloc( max_bloques , sizeof(int) ) ;
	
	// Generamos la curva 1D para obtener los tamaños de bloque
	acumulado = 0 ;
	i1 = 1 ;
	
	printf("\n\nTAMAÑO DE BLOQUES:" ) ;
	while (1)
	{
		v1 = clave[0]*i1 + clave[1]*(i1*i1) + clave[2]*(i1*i1*i1) + clave[3]*(i1*i1*i1*i1) ;
		v1 = v1%0xFFFF ;
		if ( v1 < 0 ) v1 = 0xFFFF-v1 ;
		if ( v1 < 0xFFF ) v1 = 0xFFF ;
		acumulado+= v1 ;
		if ( acumulado >= fsize )
		{
			v1 -= acumulado - fsize ;
			tabla_sizes[i1-1] = v1 ;
			printf("\n%d" , tabla_sizes[i1-1] ) ;
			break ;
		}
		tabla_sizes[i1-1] = v1 ;
		printf("\n%d" , tabla_sizes[i1-1] ) ;
		i1++ ;
	}    	
	
	num_bloques = i1 ;
	
	
	    
	printf("\n\nMaximo numero de bloques para el fichero: %d" , max_bloques ) ;
	v1 = fsize / ((0xFF + 0xF) / 2 ) ;
	printf("\nEstimacion media de bloques para el fichero: %d" , v1 ) ;
	printf("\nNumero de bloques para el fichero: %d" , num_bloques ) ;


	// Ahora ordenamos los bloques con otra curva 1D
	pool = calloc( 0xFFFF * 2 , sizeof(int) ) ;

	//i1 = 1 ;

	printf("\n\nCOORDENADAS PARA ORDENACION DE BLOQUES:") ;

	for ( i1 = 1 ; i1 <= num_bloques ; i1++ )
	{
		v1 = clave[4]*i1 + clave[5]*(i1*i1) + clave[6]*(i1*i1*i1) + clave[7]*(i1*i1*i1*i1) ;
		v1 = v1%0xFFFF ;
		if ( v1 < 0 ) v1 = 0xFFFF+v1 ;
		if ( pool[v1*2] )
			//Busqueda positiva !!!
			while ( pool[v1*2] )
			{
				v1++ ;
				v1%0xFFFF ;
			}
		pool[(v1*2)+0] = 1 ;
		pool[(v1*2)+1] = i1 ;
		printf("\n%d %d" , pool[(v1*2)+1], v1 ) ;
	}    	

	printf("\n\nORDEN DE BLOQUES:\n" ) ;

	tabla = calloc( num_bloques , sizeof(int) ) ;

	i2 = 0 ;

	// Hace falta hacer un sistema para que no siempre suela empezar
	// por el bloque 1
	for ( i1 = 0 ; i1 < 0xFFFF ; i1++ )
	{
		if ( pool[i1*2] )
		{
			tabla[ i2++ ] = pool[(i1*2)+1] ;
			printf("\n%d" , pool[(i1*2)+1] ) ;
		}
	}

	// Liberamos un poco de basura
	free(pool) ;
	
	// Generamos el fichero de salida
	fcode = fopen(filecode , "w+b") ;
    
	// Preparamos clavet
	for ( i1 = 0 ; i1 < 12 ; i1++ )
		clavet[i1] = clave[i1] ;

	tabla_clave = calloc( num_bloques , 6*4 ) ;
	tabla_clavet = calloc( num_bloques , 12 ) ;

	for ( i1 = 1 ; i1 <= num_bloques ; i1++ )
	{
		
		// Generamos la clave para el bloque con una curva 1D aplicada bit a bit
		pool = calloc( 0xFF * 1 , sizeof(int) ) ;
		
		for ( i2 = 1 ; i2 <= 12 ; i2++ )
            for ( i3 = 0 ; i3 < 8 ; i3++ )
			{
				v1 = clavet[8]*(i2) + clavet[9]*(i2*i2) + clavet[10]*(i2*i2*i2) + clavet[11]*(i2*i2*i2*i2) ;
				v1 = v1%0xFF ;
				if ( v1 < 0 ) v1 = 0xFF + v1 ;
				while ( pool[ v1 ]!=0 )
				{
					v1++ ;
					v1 = v1%0xFF ;
				}
				if ( clavet[i2-1] & ((char)(pow( 2 , i3 )) ) )
					pool[ v1 ] = 1 ;
				else
					pool[ v1 ] = 2 ;
			} ;

		// De nuevo habria que hacer algo para que el primer bit no se quedase fijo!!
		
		//printf("\n\nClave: " ) ;
		
		i4 = 0 ;
		for ( i2 = 0 ; i2 < 12 ; i2++ )
		{
			clavet[i2] = 0 ;
			for ( i3 = 0 ; i3 < 8 ; i3++ )
				while(1)
				{
					if ( pool[i4]!= 0 )
					{
						if ( pool[i4] == 1 )
                            clavet[i2] |= (int)pow(2,i3) ;
						//else
						//	clavet[i2] &= 0xFF - pow(2,i3) ;
							//exit(0) ;

						i4++ ;
						break ;
					}
					i4++ ;
				} ;
			//printf( "%i " , clavet[i2] ) ;
		} ;

		// Proceso de mutación mediante una curva 1D aplicada sobre si misma
		// Los cambios que tengan lugar durante la mutación afectarán a los
		// siguientes pasos de la mutacion!!!!!!
		for ( i2 = 0 ; i2 < 16 ; i2++ )
		{
			v1 = clavet[0]*(i2) + clavet[1]*(i2*i2) + clavet[2]*(i2*i2*i2*i2) ;
			v1 %= 96 ;
			if ( v1 < 0 ) v1 += 96 ;
			v2 = v1 / 8 ;
			v3 = v1 % 8 ;
			clavet[v2] |= (int)pow( 2 , v3 ) ;

			v1 = clavet[3]*(i2) + clavet[4]*(i2*i2) + clavet[5]*(i2*i2*i2*i2) ;
			v1 %= 96 ;
			if ( v1 < 0 ) v1 += 96 ;
			v2 = v1 / 8 ;
			v3 = v1 % 8 ;
			clavet[v2] &= 0xFF - (int)(pow( 2 , v3 )) ;
		}

        // Sacamos los valores mutados
		//printf("\nClave: " ) ;
		//for ( i2 = 0 ; i2 < 12 ; i2++ )
		//	printf("%d " , clavet[i2] ) ;

        // Liberamos memoria
		free(pool) ;

		// Ordenamos las dimensiones
		printf("\n") ;

		pool = calloc( 0xF , sizeof(int) ) ;

		for( i2 = 1 ; i2 <= 6 ; i2++ )
		{
			v1 = clavet[0]*(i2) + clavet[4]*(i2*i2) + clavet[8]*(i2*i2*i2) ;
			v1 %= 0xF ;
			if ( v1 < 0 ) v1 += 0xF ;
			while ( pool[v1] )
				v1 = (v1 +1) % 0xF ;
			pool[v1] = i2 ;
		}

		i3 = 0 ;
		for ( i2 = 0 ; i2 < 6 ; i2++ )
		{
			while( !pool[i3++] ) ;
			claved[i2][0] = (clavet[pool[i3-1]*2] & 0xF0)>>4 ;
			claved[i2][1] = (clavet[pool[i3-1]*2] & 0xF) ;
			claved[i2][2] = (clavet[(pool[i3-1]*2) + 1] & 0xF0)>>4 ;
			claved[i2][3] = (clavet[(pool[i3-1]*2) + 1] & 0xF) ;

			
			if ( claved[i2][0] % 2 )
				claved[i2][0] = -claved[i2][0] ;
			if ( claved[i2][1] % 2 )
				claved[i2][1] = -claved[i2][1] ;
			if ( claved[i2][2] % 2 )
				claved[i2][2] = -claved[i2][2] ;
			if ( claved[i2][3] % 2 )
				claved[i2][3] = -claved[i2][3] ;
			claved[i2][0] /= 2 ;
			claved[i2][1] /= 2 ;
			claved[i2][2] /= 2 ;
			claved[i2][3] /= 2 ;

			tabla_clave[ (i1-1)*6*4 + i2*4 +0 ] = claved[i2][0] ;
			tabla_clave[ (i1-1)*6*4 + i2*4 +1 ] = claved[i2][1] ;
			tabla_clave[ (i1-1)*6*4 + i2*4 +2 ] = claved[i2][2] ;
			tabla_clave[ (i1-1)*6*4 + i2*4 +3 ] = claved[i2][3] ;
			
			//printf("%d\t%d\t%d\t%d\n", claved[i2][0] , claved[i2][1] , claved[i2][2] , claved[i2][3] ) ;
		}

		for ( i2 = 0 ; i2 < 12 ; i2++ )
			tabla_clavet[ (i1-1)*12 + i2 ] = clavet[i2] ;

		free(pool) ;
//		free( poolc ) ;
	}

	// Descodificamos los bloques
	for ( num_bloque = 1 ; num_bloque <= num_bloques ; num_bloque++ )
	{
		printf("\n\nDESCODIFICANDO BLOQUE %d" , num_bloque ) ;
		
		// Leemos el bloque en memoria
		//num_bloque = tabla[i1-1] ;
		for ( i1 = 0 ; i1 < num_bloques ; i1++ )
			if ( tabla[ i1 ] == num_bloque )
				break ;

		size_bloque = tabla_sizes[ num_bloque-1 ] ;
		poolc = calloc( size_bloque , 1 ) ;
		acumulado = 0 ;
		for ( i2 = 0 ; i2 < i1 ; i2++ )
			acumulado += tabla_sizes[ tabla[i2]-1 ] ;
		//printf("acumulado: %d\n" , acumulado ) ;
		fseek( fclaro , acumulado , SEEK_SET ) ;		
		fread( poolc , 1 , size_bloque , fclaro ) ;

		// Cargamos la clave para el bloque
		for ( i2 = 0 ; i2 < 12 ; i2++ )
			clavet[i2] = tabla_clavet[ (i1)*12 + i2 ] ;

		// Cargamos las claved para el bloque
		for ( i2 = 0 ; i2 < 6 ; i2++ )
			for ( i3 = 0 ; i3 < 4 ; i3++ )
				claved[i2][i3] = tabla_clave[ (i1)*6*4 + i2*4 + i3 ] ;



		// Calculamos tamaños de cada dimension
		//printf("\n\n") ;
		for ( i2 = 1 ; i2 <= 6 ; i2++ )
		{
			v1 = clavet[1]*(i2) + clavet[5]*(i2*i2) + clavet[9]*(i2*i2*i2) ;
			v1 %= 4 ;
			if ( v1 < 0 ) v1 += 4 ;
			size_dim[i2-1] = v1 + 12 ;
			//printf("%d " , size_dim[i2-1] ) ;
		}
		
        // Generamos el espacio 6D
		size_poolc = size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3]*size_dim[4]*size_dim[5] ;
		pool = calloc( size_poolc , sizeof(int) ) ;

		// Generamos la curva 6D
		for ( i2 = 1 ; i2 <= size_bloque ; i2++ )
		{
			v1 = claved[0][0]*(i2) + claved[0][1]*(i2*i2) + claved[0][2]*(i2*i2*i2) + claved[0][3]*(i2*i2*i2*i2) ;
			v2 = claved[1][0]*(i2) + claved[1][1]*(i2*i2) + claved[1][2]*(i2*i2*i2) + claved[1][3]*(i2*i2*i2*i2) ;
			v3 = claved[2][0]*(i2) + claved[2][1]*(i2*i2) + claved[2][2]*(i2*i2*i2) + claved[2][3]*(i2*i2*i2*i2) ;
			v4 = claved[3][0]*(i2) + claved[3][1]*(i2*i2) + claved[3][2]*(i2*i2*i2) + claved[3][3]*(i2*i2*i2*i2) ;
			v5 = claved[4][0]*(i2) + claved[4][1]*(i2*i2) + claved[4][2]*(i2*i2*i2) + claved[4][3]*(i2*i2*i2*i2) ;
			v6 = claved[5][0]*(i2) + claved[5][1]*(i2*i2) + claved[5][2]*(i2*i2*i2) + claved[5][3]*(i2*i2*i2*i2) ;

            v1 %= size_dim[0] ;
			v2 %= size_dim[1] ;
			v3 %= size_dim[2] ;
			v4 %= size_dim[3] ;
			v5 %= size_dim[4] ;
			v6 %= size_dim[5] ;

			if ( v1 < 0 ) v1 += size_dim[0] ;
			if ( v2 < 0 ) v2 += size_dim[1] ;
			if ( v3 < 0 ) v3 += size_dim[2] ;
			if ( v4 < 0 ) v4 += size_dim[3] ;
			if ( v5 < 0 ) v5 += size_dim[4] ;
			if ( v6 < 0 ) v6 += size_dim[5] ;

			v[1] = v1 ;
			v[2] = v2 ;
			v[3] = v3 ;
			v[4] = v4 ;
			v[5] = v5 ;
			v[6] = v6 ;

			while ( pool[(v[1] + v[2]*size_dim[0] + v[3]*size_dim[0]*size_dim[1] + v[4]*size_dim[0]*size_dim[1]*size_dim[2] + v[5]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3] + v[6]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3]*size_dim[4])  ] )
			{
				v[1] = (v[1] + 1) % size_dim[0] ;
				if ( v[1] == v1 )
				{
					v[2] = (v[2] + 1) % size_dim[1] ;
					if ( v[2] == v2 )
					{
						v[3] = (v[3] + 1) % size_dim[2] ;
						if ( v[3] == v3 )
						{
							v[4] = (v[4] + 1) % size_dim[3] ;
							if ( v[4] == v4 )
							{
								v[5] = (v[5] + 1) % size_dim[4] ;
								if ( v[5] == v5 )
									v[6] = (v[6] + 1) % size_dim[5] ;
							}
						}
					}
				}
			}
			//printf("A") ;
			dir_dim = (v[1] + v[2]*size_dim[0] + v[3]*size_dim[0]*size_dim[1] + v[4]*size_dim[0]*size_dim[1]*size_dim[2] + v[5]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3] + v[6]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3]*size_dim[4])  ;
			pool[dir_dim] = i2 ;
			
		}

		// Reservamos el bloque de memoria donde vamos a reordenar los datos
		poole = calloc( size_bloque , 1 ) ;
        i2 = 0 ;

		acumulado = 0 ;
		// Y ahora leemos la información
		for ( v[6] = 0 ; v[6] < size_dim[5] ; v[6]++ )
			for ( v[5] = 0 ; v[5] < size_dim[4] ; v[5]++ )
				for ( v[4] = 0 ; v[4] < size_dim[3] ; v[4]++ )
					for ( v[3] = 0 ; v[3] < size_dim[2] ; v[3]++ )
						for ( v[2] = 0 ; v[2] < size_dim[1] ; v[2]++ )
							for ( v[1] = 0 ; v[1] < size_dim[0] ; v[1]++ )
							{
								dir_dim = ( v[1] + v[2]*size_dim[0] + v[3]*size_dim[0]*size_dim[1] + v[4]*size_dim[0]*size_dim[1]*size_dim[2] + v[5]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3] + v[6]*size_dim[0]*size_dim[1]*size_dim[2]*size_dim[3]*size_dim[4]) ;
								if (pool[dir_dim] != 0)
								{
									acumulado++ ;
									//printf("\n%d %d %d %d %d %d : %d", v[1], v[2], v[3], v[4], v[5], v[6] , dir_dim) ;
									//fwrite( &poole[dir_dim+1] , 1 , 1 , fcode ) ;
									poole[pool[dir_dim]-1] = poolc[i2++] ;

								}
							} ;

		fwrite( poole , 1 , size_bloque , fcode ) ;
		//fwrite( poolc , 1 , size_bloque , fcode ) ;
		printf("\nEscrito: %d", acumulado ) ;

							

														




		printf("\n\n" ) ;



		// Liberamos memoria
		free( poole ) ;
		free( poolc ) ;
		free( pool ) ;
	}


	fclose( fclaro ) ;
	fclose( fcode ) ;
}
