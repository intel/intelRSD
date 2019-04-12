grammar UnificationPattern;

unificationPatternFile: ((WhiteSpaces? Comment* WhiteSpaces?) unificationPattern WhiteSpaces? Comment?)* unificationPattern?;

unificationPattern: matchingPattern WhiteSpaces '->' WhiteSpaces rewritingPattern;

matchingPattern: ('[' existingResourcePattern ']')? ('/' segmentMatchingPattern)+ ;

existingResourcePattern: ('/' segmentMatchingPattern)+ ;

segmentMatchingPattern
    : SimpleIdentifier #constSegmentMatchingPattern
    | '{}'             #variableSegmentMatchingPattern
    ;

rewritingPattern: existingResourceSubstitution? ('/' segmentRewritingPattern)+;

existingResourceSubstitution: '[]';

segmentRewritingPattern
    : SimpleIdentifier              #constSegmentRewritingPattern
    | argumentSubstitutionPattern   #argSubstitutionPattern
    ;

argumentSubstitutionPattern: '{' pathSegments '}' ;

pathSegments: abstractPathSegment
        | abstractPathSegment WhiteSpaces pathSegments;


abstractPathSegment: relativeResolvablePath | absoluteResolvablePath | constFragment | calculableSegment;

calculableSegment: '$' transformingFunctionName '(' (pathFragment (',' WhiteSpaces pathFragment)*)? ')';
relativeResolvablePath: pathFragment;
absoluteResolvablePath: pathSegment pathFragment;
constFragment: '\'' SimpleIdentifier '\'';
pathFragment: ('#/' SimpleIdentifier)+;
pathSegment: ('/' SimpleIdentifier)+;
transformingFunctionName: SimpleIdentifier;

WhiteSpaces: [ \t\n]+;
SimpleIdentifier: ('_'|'.'|'+'|':'|'%'|[A-Za-z0-9-])+;
Comment: '//' ~('\n')*;




