
customer
{
        name STRING                        ; Name of the customer
        id @UINT                           ; Internal customer id
        address                            ; Address of the customer
        {
                street STRING              ; Street and house number
                state STRING               ; State
                country USHORT(49)         ; Countrycode, default switzerland
        }
        links[]                            ; List of links to other customers
        {
                customerid UINT            ; Id of the linked customer
                description STRING         ; Description of this relation
        }
}

