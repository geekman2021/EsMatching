<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />

    <style>
      page[size="A4"] {
        width:  29.7cm;
        height: 21cm;
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

      /* section {
        padding-top: 15px;
        padding-left: 0px;
        float: left;
      } */
    </style>
    
    <title>Document</title>
  </head>
  <body>
    <page size="A4" style="margin-top: 10%;">
      <div style="padding: 20px">
        <header >
            <div class="header-left">
              <img src="<?php echo base_url(); ?>assets/images/logo.png" alt="logo" > <br><br>
              DEMANDE ETABLIE PAR : 09944 DDM <br>
            </div>
            <div class="header-right" style="padding-left: 20%; font-size: 0.7rem;">
                Référence N° :<?= $reference ?>/09044/<?php echo $operateur; ?>/<?php echo $nom; ?>/<?php echo $annee; ?> <br>
                DATE :<?php echo $date; ?> <br>
                DESTINATAIRE : DDC <br>
                REGULARISATION A FAIRE PAR : DDC <br>
            </div>
        </header>
           <!-- <section>
             
           </section> -->
              
            <table border="1" cellpadding="8" cellspacing="0" style="width: 100%">
            <caption style="font-size: 0.8rem; padding-top: 20px;"><b> <u> FICHE DE REGULARISATION ( Fr 10) </u> </b><br> <br> DATE D'ENVOI: __/__/____ </caption>
              <thead style="padding-top: 12px;">
                
                <tr style="text-align: center;">
                  <th rowspan="2">DATE DE L'OPERATION</th>
                  <th rowspan="2">TYPE DE L'OPERATION</th>
                  <th rowspan="2">NOM DE L'ECRAN SAISI</th>
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
                <td><?php echo $date_oper;  ?></td>
                <td>TRANSFERT D'ARGENT VIA  <?php echo $mobile_money; ?> <?php echo $libelle; ?></td>
                <td></td>
                <td><b><?php echo $ref_igor; ?></b></td>
                <td><?php echo $compte_client;?></td>
                <td><?php echo $montant; ?></td>
                <td>09194120000</td>
                <td><?php echo $montant; ?></td>
                <td>
                <?php if($mobile_money ==="ORANGE") {  ?>
                  Une transaction de <b><?php echo $montant; ?> </b> MGA a été effectuée par le client le <b><?php echo $date_oper; ?></b> d'Orange vers IGOR sous la référence <b><?php echo $ref_igor; ?></b>. L’opération annulation par <?php echo $mobile_money; ?> n’a pas été prise en compte par IGOR suivant la confirmation d'Orange en pièce jointe.</td>
                <?php } else if($operateur ==="AIRTEL MONEY") { ?> 
                  Une transaction de <b><?php echo $montant; ?> </b> MGA a été effectuée par le client le <b><?php echo $date_oper; ?></b> de IGOR vers <?php echo $mobile_money; ?> sous la référence <b><?php echo $ref_igor; ?></b>. L’opération sous référence n’a pas été prise en compte par <?php echo $mobile_money; ?> suivant la confirmation de Airtel  en pièce jointe.		</td>
                <?php } else { ?>
                  Une transaction de <b><?php echo $montant; ?> </b> MGA a été effectuée par le client le <b><?php echo $date_oper; ?></b> de IGOR vers Mvola sous la référence <b><?php echo $ref_igor; ?></b>. L’opération sous référence n’a pas été prise en compte par <?php echo $mobile_money; ?> suivant la confirmation de Mvola  en pièce jointe.		</td>
                <?php } ?>
                 
                
                <td><b> Annulation de la réference susmentionnée(<?php echo $ref_igor; ?>) du <?php echo $date_oper; ?></b></td>
              </tr>
              <tr>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td><?php echo $compte_client;?></td>
                <td><?php echo $montant_comm; ?></td>
                <td><?php echo $compte_comm; ?></td>
                <td><?php echo $tax; ?></td>
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
                <td>09315120008</td>
                <td><?php echo $tva; ?></td>
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
                <td colspan="2" rowspan="3" style="text-align:left;"><b>DECISION-DCCI/DCR</b> <br> <br> <br> <br> <br> <br> Date</td>
              </tr>
              <tr>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
              </tr>
              <tr>
                <td colspan="2"><?php echo $responsable; ?><br>
                <?php echo $place; ?> <br>
                    Date : <?php echo $date; ?>
                </td>
                <td>Date: </td>
                <td> </td>
                <td></td>
              </tr>

              <tr>
                <td colspan="2"><b>2 COMPTE RENDU DE REGULARISATION <b></td>
                <td></td>
                <td></td>
                <td colspan="6" style= "text-align: center;"><b>DATE D'ENVOI A L'UNITE DEMANDERESSE:</b></td>
              </tr>
              <tr>
                <td  style="text-align: center;"><b>DATE DE L'OPERATION DE REGULARISATION</b></td>
                <td  style="text-align: center;"><b>NOM DE L'ECRAN DE SAISIE LORS DE LA REGULARISATION</b></td>
                <td colspan="2"  style="text-align: center;"><b>REFERENCE DE L'OPERATION DE REGULARISATION</b></td>
                <td colspan="2"  style="text-align: center;"><b>REFERENCE DE L'OPERATION DE REGULARISATION</b></td>
                <td colspan="2" style="text-align: center;"><b>INTERVENTIONS</b></td>
                <td colspan="2" style="text-align: center;"><b>AUTRES OBSERVATIONS:</b></td>
              </tr>
              <tr>
                <td style="height: 50px;"></td>
                <td style="height: 50px;"></td>
                <td colspan="2" style="height: 50px;"></td>
                <td colspan="2" style="height: 50px;"></td>
                <td colspan="2" style="height: 50px;"></td>
                <td colspan="2" style="height: 50px;"></td>

              </tr>
            </tr> 
          
          </tr>
          
          </tbody>
        <tr>
          
       
    </tbody>
    </table>
    <span style="font-size: 0.5rem;">(*) : rayer les mentions inutiles	</span>
    <div>
      <p style="font-size: 0.5rem;"><u> COPIE </u> : COMPTABILITÉ-AUDIT INTERNE</p>
    </div>
    
        </div>
      </div>
    </page>
  </body>


</html>
