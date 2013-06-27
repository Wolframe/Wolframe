#! /usr/bin/env python
#
# Script to create debian/control for package boost-defaults

from deb822 import Deb822
import re

gBoostVersion = None


class BoostVersion:
    def __init__(self, version):
        (self.Major,self.Minor,self.Revision) = version.split('.')
        self.PackageVersion = self.Major + '.' + self.Minor
        self.SharedObjectVersion = version
    def versionedSubPackage(self, subpkg):
        '''Given a subpackage name (subpkg) such as "dev",
        returns the versioned package name, libboost@Major.Minor@-@subpkg@.'''
        return 'libboost' + self.PackageVersion + '-' + subpkg
    def containsPackageVersion(self, string):
        '''Return true if 'string' contains the Package version string.'''
        return re.search(self.PackageVersion, string) is not None
    def containsSharedObjectVersion(self, string):
        '''Return true if 'string' contains the Shared Object version string.'''
        return re.search(self.SharedObjectVersion, string) is not None
    def stripVersion(self, string):
        '''Remove PackageVersion or SharedObjectVersion if contained in 'string'.'''
        return self.replaceVersion(string,'')
    def replaceVersion(self, string, replacement):
        '''Replace either PackageVersion or SharedObjectVersion if contained in 'string',
        with 'replacement'.'''
        string = re.sub(self.SharedObjectVersion, replacement, string)
        string = re.sub(self.PackageVersion, replacement, string)
        return string

def appendFirstLine(string, suffix):
    '''Given a multiline string, return a new string
    with suffix appended to the first line.'''
    lines = string.split('\n')
    lines[0] += suffix
    return '\n'.join(lines)

def deleteKey(paragraph, key):
    if paragraph.has_key(key):
        del paragraph[key]

def processSourceParagraph(p):
    p['Source'] = 'boost-defaults'
    p['Build-Depends'] = 'debhelper (>= 8), ' + \
        gBoostVersion.versionedSubPackage('dev') + ' (>= ' + gBoostVersion.SharedObjectVersion + ')'
    deleteKey(p, 'Build-Conflicts')
    p['Vcs-Browser'] = 'http://svn.debian.org/wsvn/pkg-boost/boost-defaults/trunk/'
    p['Vcs-Svn'] = 'svn://svn.debian.org/svn/pkg-boost/boost-defaults/trunk'

def genericPackageName(pkgName):
    '''Given an input package name of the form x1-x2-...-yVERSION-z,
    return an unversioned variant of form x1-x2-...-y-z.'''
    nameComponents = pkgName.split('-')
    lastComp = nameComponents[-1]
    if not lastComp in ['dbg','dev','doc']:
        return None
    return '-'.join(map(gBoostVersion.stripVersion, nameComponents))

def dependsLibDev(p, versionedName):
    '''Set package Depends for a library -dev package.'''
    return '${misc:Depends}, ' + versionedName

def dependsDbg(p, versionedName):
    '''Set package Depends for libboost-dbg.'''
    return 'libboost-dev (= ${binary:Version}), ' + dependsLibDev(p, versionedName)

def dependsAllDev(p, versionedName):
    '''Set package Depends for libboost-all-dev.'''
    return gBoostVersion.stripVersion(p['Depends'])

def processPackageParagraph(p):
    versionedName = p['Package']
    genericName = genericPackageName(versionedName)
    if genericName is None:
        return False
    p['Package'] = genericName
    if genericName == 'libboost-dbg':
        depends = dependsDbg(p, versionedName)
    elif genericName == 'libboost-all-dev':
        depends = dependsAllDev(p, versionedName)
    else:
        depends = dependsLibDev(p, versionedName)
    p['Depends'] = depends
    if genericName == 'libboost-dev':
        p['Suggests'] = 'libboost-doc'
    else:
        deleteKey(p, 'Suggests')
    deleteKey(p, 'Recommends')
    deleteKey(p, 'Conflicts')
    deleteKey(p, 'Replaces')
    p['Description'] = appendFirstLine(p['Description'], ' (default version)') \
                       + '''
 .
 This package is a dependency package, which depends on Debian\'s default
 Boost version (currently ''' + gBoostVersion.PackageVersion + ').'
    return True

def printParagraph(p):
    for key in p.keys():
        print "%s: %s" % (key, p[key])

def processControl():
    firstParagraph = True
    for paragraph in Deb822.iter_paragraphs(open('control')):
        if firstParagraph:
            processSourceParagraph(paragraph)
            printParagraph(paragraph)
            firstParagraph = False
        else:
            if processPackageParagraph(paragraph):
                print
                printParagraph(paragraph)



gBoostVersion = BoostVersion('1.48.0')
processControl()
