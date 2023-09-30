<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <!-- <link rel="stylesheet" href="./facture.css" /> -->
    <style>
      page[size="A4"] {
        width: 21cm;
        height: 17.5cm;
      }

      page {
        background: white;
        display: block;
        margin: 0 auto;
        margin-bottom: 0.5cm;
        box-shadow: 0 0 0.5cm rgba(0, 0, 0, 0.5);
      }
      body {
        background: rgb(204, 204, 204);
      }

      table {
        font-size: 0.5rem
      }
      .header-left {
        float: left;
        padding-top: 20px;

      }
      .header-right {
        float: right;
        padding-right: 4vw;
        padding-top: 20px;
      }

      section {
        padding-top: 15px;
        padding-left: 0px;
        float: left;
      }
    </style>
    
    <title>Document</title>
  </head>
  <body>
    <page size="A4" style="margin-top: 10%;">
      <div style="padding: 10px">
        <header >
            <div class="header-left">
                    <img src="<?php echo base_url(); ?>assets/images/logo.png" alt="logo" >
            </div>
            <div class="header-right" style="padding-left: 20%; font-size: 0.7rem;">
                    Référence N° :<?php echo $reference ?>/09044/AIRTEL/MRH/<?php echo $annee; ?> <br>
                    DATE :<?php echo $date; ?> <br>
                    DESTINATAIRE : DDC <br>
                    REGULARISATION A FAIRE PAR : DDC <br>
            </div>
        </header>
           <section>
              <div  style="font-size: 0.8rem;">
                DEMANDE ETABLIE PAR : 09944 DDM <br>
                Date d'Envoi <b>__/__/_____</b>

              </div>
           </section>
              
            <table border="1" cellpadding="8" cellspacing="0">
            <caption style="font-size: 0.8rem; padding-top: 20px;"><b> FICHE DE REGULATION MONTANT SUPERIEUR A 100 000 MG</b><br></caption>
              

              <thead style="padding-top: 12px;">
                
                <tr style="text-align: center;">
                  <th rowspan="2">Date DE L'OPERATION</th>
                  <th rowspan="2">TYPE DE L'OPERATION</th>
                  <th rowspan="2">NOM DE L'ECRAN SAISIE</th>
                  <th rowspan="2">REF DE L'OPERATION</th>
                  <th colspan="2">DEBIT</th>
                  <th colspan="2">CREDIT</th>
                  <th rowspan="2">Description précise de l’anomalie ou de l’erreur </th>
                  <th rowspan="2">Régularisation démandée</th>
                </tr>
                <tr>
                  <th>COMPTE</th>
                  <th>MONTANT</th>
                  <th>COMPTE</th>
                  <th>MONTANT</th>
                </tr>
              </thead>
            <tbody>
              <tr>
                <td><?php echo $date;  ?></td>
                <td>transfer d'argent via airtel Money operation <?php echo $oper; ?></td>
                <td></td>
                <td><?php echo $ref_igor; ?></td>
                <td><?php echo $compte;?></td>
                <td><?php echo $montant; ?></td>
                <td>09194120000</td>
                <td></td>
                <td>Une transaction de <b><?php echo $montant; ?> </b> MGA a été effectuée par le client le <b><?php echo $date_oper; ?></b> de IGOR vers Airtel money sous la référence <b><?php echo $ref_igor; ?></b>. L’opération sous référence n’a pas été prise en compte par Airtel money suivant la confirmation de Airtel  en pièce jointe.		</td>
                <td>Annulation de la réference surmentionnée <b><?php echo $ref_igor; ?></b> du <b><?php echo $date_oper; ?></b></td>
              </tr>
              <tr>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td>compte</td>
                <td>250,000</td>
                <td>0974080005</td>
                <td>208,33</td>
                <td></td>
                <td></td>
              </tr>

              <tr>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td>0974080005</td>
                <td>41,67</td>
                <td></td>
                <td></td>
              </tr>
              <tr>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
              </tr>
              <tr style="text-align: center;">
                <td colspan="2" style="text-align: center;">RESPONSABLE de l'UNITE</td>
                <td colspan="3" style="text-align: center;">DDON-FINANCE (*)</td>
                <td colspan="3" rowspan="3" style="text-align:left;">DECISION-DGA/PSF <br> <br> <br> <br> <br> <br> Date</td>
                <td colspan="2" rowspan="3" style="text-align:left;"><b>DECISION-DCC/DCR</b> <br> <br> <br> <br> <br> <br> Date</td>
              </tr>
              <tr>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
              </tr>
              <tr>
                <td colspan="2"> RANDRIANAIVOSON Lazanirainy <br>
                    Directeur Déléguée à la Monétique (p.i) <br>

                    Date :21/09/23
                </td>
                <td>Date: </td>
                <td> </td>
                <td></td>
              </tr>

              <tr>
                <td colspan="3">2 COMPTE RENDU DE REGULARISATION DE </td>
                <td></td>
                <td colspan="6"><b>DATE D'ENVOI A L'UNITE DEMANDERESSE:</b></td>
              </tr>
              <tr>
               
                <td>DATE DE L'OPERATION DE REGULARISATION</td>
                <td colspan="2">NOM DE L'ECRAN DE SAISIE LORS DE LA REGULARISATION</td>
                <td colspan="2"> REFERENCE DE L'OPERATION DE REGULARISATION</td>
                <td colspan="2"> REFERENCE DE L'OPERATION DE REGULARISATION</td>
                <td>INTERVENTIONS</td>
                <td colspan="2"><b>AUTRES OBSERVATIONS:</b></td>
              </tr>
              <tr>
               
               <td></td>
               <td colspan="2"></td>
               <td colspan="2"></td>
               <td colspan="2"></td>
               <td></td>
               <td colspan="2"><b></b></td>
             </tr>
            </tbody>
        <tr>
          
       
    </tbody>
    </table>
    <span style="font-size: 0.5rem;">(*) : rayer les mentions inutiles	</span>
    <p style="font-size: 0.5rem;">COPIE : COMPTABILITÉ-AUDIT INTERNE</p>
        </div>
      </div>
    </page>
  </body>


</html>
