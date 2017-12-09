properties([
		parameters([
				choice(choices: 'Debug\nRelease\nRelWithDebInfo\nMinSizeRel', description: 'CMake build type', name: 'BUILD_TYPE')
		])
])

pipeline {
	agent {
		label 'qt && cmake'
	}
	stages {
		stage('Build') {
			steps {
				sh '''
					mkdir build
					cd build
					cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
					make -j
				'''
			}
			post {
				success {
					warnings canResolveRelativePaths: false, consoleParsers: [[parserName: 'GNU Make + GNU C Compiler (gcc)']], useStableBuildAsReference: true
				}
			}
		}
		stage('Analyze') {
			steps {
				sh '''
					cppcheck --enable=warning,style --inconclusive -q --template="{file},{line},{severity},{id},{message}" -I build/src -I src src
				'''
			}
			post {
				success {
					warnings canResolveRelativePaths: false, consoleParsers: [[parserName: 'CppCheck (custom)']], useStableBuildAsReference: true
				}
			}
		}
		stage('Deploy') {
			when {
				environment name: 'BUILD_TYPE', value: 'Release'
			}
			steps {
				archiveArtifacts artifacts: 'build/bin/**', onlyIfSuccessful: true
			}
		}
	}
}
